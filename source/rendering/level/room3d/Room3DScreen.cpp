
#include <utils/code_editor/CodeEditor.h>
#include <level/Level.h>
#include <game/dibidab.h>
#include <graphics/orthographic_camera.h>
#include <utils/quad_renderer.h>
#include "Room3DScreen.h"
#include "../../../generated/Camera.hpp"
#include "../../../generated/Light.hpp"
#include "../../../game/Game.h"
#include "../../../ecs/systems/graphics/3d/CustomShaderSystem.h"

#include <generated/Inspecting.hpp>

Room3DScreen::Room3DScreen(Room3D *room)
        :
        room(room),
        renderer(room),
        inspector(*room, "Room3D"),

        defaultShader(
            "default shader",
            "shaders/3d/default.vert", "shaders/3d/default.frag"
        ),
        riggedShader(
            "rigged shader",
            "shaders/3d/rigged.vert", "shaders/3d/default.frag"
        ),
        skyShader(
            "sky shader",
            "shaders/fullscreen_quad.vert", "shaders/3d/sky.frag"
        )
{
    assert(room != NULL);

    inspector.createEntity_showSubFolder = "3d_models";
}

void Room3DScreen::render(double deltaTime)
{
    gu::profiler::Zone z("Room3D");

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!room->camera)
        return;

    Room3DRenderer::Context finalImg { *room->camera, defaultShader, riggedShader };
    finalImg.time = room->getLevel().getTime();
    finalImg.mask = ~0u;
    finalImg.skyShader = &skyShader;

    if (room->entities.valid(room->cameraEntity))
        if (auto *cp = room->entities.try_get<CameraPerspective>(room->cameraEntity))
            finalImg.mask = cp->visibilityMask;

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderer.renderRoom(finalImg);

    renderDebugStuff(deltaTime);
}

void Room3DScreen::onResize()
{
}

int debugTextI = 0;

void Room3DScreen::renderDebugStuff(double deltaTime)
{
    if (!dibidab::settings.showDeveloperOptions)
        return;

    gu::profiler::Zone z("debug");
    assert(room->camera != NULL);
    auto &cam = *room->camera;
    lineRenderer.projection = cam.combined;

    debugTextI = 0;

    {
        // x-axis:
        lineRenderer.line(vec3(cam.position.x - 1000, 0, 0), vec3(cam.position.x + 1000, 0, 0), mu::X);
        // y-axis:
        lineRenderer.line(vec3(0, cam.position.y - 1000, 0), vec3(0, cam.position.y + 1000, 0), mu::Y);
        // z-axis:
        lineRenderer.line(vec3(0, 0, cam.position.z - 1000), vec3(0, 0, cam.position.z + 1000), mu::Z);
    }
    debugLights();
    if (Game::settings.graphics.debugArmatures)
        debugArmatures();

    gizmoRenderer.beginFrame(deltaTime, vec2(gu::width, gu::height), cam);
    inspector.drawGUI(&cam, lineRenderer, gizmoRenderer);
    gizmoRenderer.endFrame(cam);

    ImGui::BeginMainMenuBar();

    if (ImGui::BeginMenu("Room3D"))
    {
        ImGui::Separator();

        ImGui::Checkbox("Show armatures", &Game::settings.graphics.debugArmatures);

        ImGui::Separator();
        if (ImGui::MenuItem("View as JSON"))
        {
            auto &tab = CodeEditor::tabs.emplace_back();
            tab.title = room->name.empty() ? "Untitled room" : room->name;
            tab.title += " (READ-ONLY!)";

            json j;
            room->toJson(j);
            tab.code = j.dump(2);
            tab.languageDefinition = TextEditor::LanguageDefinition::C();
            tab.revert = [j] (auto &tab) {
                tab.code = j.dump(2);
            };
            tab.save = [] (auto &tab) {
                std::cerr << "Saving not supported" << std::endl;
            };
        }
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();

}

Room3DScreen::~Room3DScreen()
{
}

void Room3DScreen::debugLights()
{
    gu::profiler::Zone z("lights");
    // directional lights:
    room->entities.view<Transform, DirectionalLight>().each([&](auto e, Transform &t, DirectionalLight &dl) {

        auto transform = Room3D::transformFromComponent(t);
        vec3 direction = transform * vec4(-mu::Y, 0);

        lineRenderer.axes(t.position, .1, vec3(1, 1, 0));
        for (int i = 0; i < 100; i++)
            lineRenderer.line(t.position + direction * float(i * .2), t.position + direction * float(i * .2 + .1), vec3(1, 1, 0));

        if (auto name = room->getName(e))
            debugText(name, t.position);
    });

    // point lights:
    room->entities.view<Transform, PointLight>().each([&](auto e, Transform &t, PointLight &pl) {
        lineRenderer.axes(t.position, .1, vec3(1, 1, 0));
        if (auto name = room->getName(e))
            debugText(name, t.position);
    });
}

void Room3DScreen::debugArmatures()
{
    gu::profiler::Zone z("armatures");
    glDisable(GL_DEPTH_TEST);
    glLineWidth(2.f);
    room->entities.view<Transform, RenderModel, Rigged>().each([&](auto e, Transform &t, RenderModel &rm, Rigged &rig) {
        auto &model = room->models[rm.modelName];
        if (!model) return;

        auto transform = Room3D::transformFromComponent(t);

        for (auto &modelPart : model->parts)
        {
            if (!modelPart.armature) continue;

            auto &arm = *modelPart.armature.get();

            int depth = 0;
            std::function<void(SharedBone &, mat4)> renderBone;
            renderBone = [&] (SharedBone &bone, mat4 globalParent) {
                depth++;

                auto global = rig.bonePoseTransform[bone] * inverse(bone->inverseBindMatrix);
                global = transform * global;

                vec3 parentJointPos = globalParent * vec4(mu::ZERO_3, 1);
                vec3 jointPos = global * vec4(mu::ZERO_3, 1);
                vec3 debugOffset = global * vec4(0, .2, 0, 1);

                lineRenderer.axes(jointPos, .05, vec3(1));

                debugText(bone->name, debugOffset);

                vec3 color = std::vector<vec3>{
                        vec3(52, 235, 164),
                        vec3(233, 166, 245),
                        vec3(242, 214, 131)
                }[depth % 3] / 255.f;

                if (depth > 1)
                    lineRenderer.line(parentJointPos, jointPos, color);
                if (bone->children.empty())
                    lineRenderer.line(jointPos, debugOffset, mu::X);

                for (auto &child : bone->children)
                    renderBone(child, global);

                depth--;
            };
            for (auto &bone : arm.bones)
            {
                if (!bone->parent)
                    renderBone(bone, mat4(1));
            }


            break;
        }
    });
    glEnable(GL_DEPTH_TEST);
    glLineWidth(1.f);
}

void Room3DScreen::debugText(const std::string &text, const vec3 &pos)
{
    bool inScreen = false;
    assert(room->camera != NULL);
    vec2 screenPos = room->camera->projectPixels(pos, inScreen);
    if (inScreen)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
        ImGui::SetNextWindowBgAlpha(0);
        ImGui::SetNextWindowPos(ImVec2(screenPos.x - 5, screenPos.y - 5));
        ImGui::SetNextWindowSize(ImVec2(200, 30));
        ImGui::Begin(("__debugtextpopup__" + std::to_string(debugTextI++)).c_str(), NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoInputs);
        ImGui::SetWindowFontScale(.9);
        ImGui::Text("%s", text.c_str());
        ImGui::End();
        ImGui::PopStyleVar();
    }
}
