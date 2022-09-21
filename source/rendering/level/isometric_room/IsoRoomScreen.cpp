
#include "IsoRoomScreen.h"
#include "../../ui/UIScreenWidgets.h"
#include "../../../level/isometric_room/IsoTileMap.h"
#include "../../../game/Game.h"

#include <game/dibidab.h>

IsoRoomScreen::IsoRoomScreen(IsoRoom *room)
  : room(room),
    entityInspector(*room, "IsoRoom"),
    camera(0.1f, 1000.f, 0.0f, 0.0f),
    tileMapMeshGenerator(&room->getTileMap()),
    tileMapShader("tileMapShader", "shaders/iso/tile_map.vert", "shaders/iso/tile_map.frag")
{
    entityInspector.createEntity_showSubFolder = "simulation";
}

void IsoRoomScreen::render(double deltaTime)
{
    gu::profiler::Zone z("IsoRoom");

    // set opengl settings and clear canvas
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // update camera
    camera.position = -normalize(room->cameraDirection) * 512.0f;
    vec3 camRightDir = normalize(cross(camera.position, mu::Y));

    camera.position = rotate(camera.position, Game::settings.graphics.isometricAngle * mu::DEGREES_TO_RAD, camRightDir);
    camera.lookAt(mu::ZERO_3);
    camera.position.x += room->cameraFocus.x;
    camera.position.z += room->cameraFocus.z;
    camera.update();
    lineRenderer.projection = camera.combined;

    // update terrain mesh:
    showTerrainEditor();
    tileMapMeshGenerator.update();

    // render triangles without using indices:
    tileMapShader.use();
    glUniformMatrix4fv(tileMapShader.location("mvp"), 1, GL_FALSE, &camera.combined[0][0]);
    Game::spriteSheet->fbo->colorTexture->bind(0, tileMapShader, "spriteSheet");
    for (uint chunkX = 0; chunkX < tileMapMeshGenerator.getNrOfChunksAlongXAxis(); chunkX++)
    {
        for (uint chunkZ = 0; chunkZ < tileMapMeshGenerator.getNrOfChunksAlongZAxis(); chunkZ++)
        {
            auto &mesh = tileMapMeshGenerator.getMeshForChunk(chunkX, chunkZ);
            mesh->renderArrays(GL_TRIANGLES, mesh->nrOfVertices());
        }
    }

    renderDebugStuff(deltaTime);
}

void IsoRoomScreen::onResize()
{
    Screen::onResize();
    camera.viewportWidth = float(gu::widthPixels) / float(Game::settings.graphics.pixelsPerMeter);
    camera.viewportHeight = float(gu::heightPixels) / float(Game::settings.graphics.pixelsPerMeter);
    camera.update();
}

void IsoRoomScreen::renderDebugStuff(double deltaTime)
{
    if (!dibidab::settings.showDeveloperOptions)
        return;

    gu::profiler::Zone zone("debug");

    entityInspector.drawGUI(nullptr, lineRenderer);

    // axes:
    {
        // x-axis:
        lineRenderer.line(vec3(camera.position.x - 1000, 0, 0), vec3(camera.position.x + 1000, 0, 0), mu::X);
        // y-axis:
        lineRenderer.line(vec3(0, camera.position.y - 1000, 0), vec3(0, camera.position.y + 1000, 0), mu::Y);
        // z-axis:
        lineRenderer.line(vec3(0, 0, camera.position.z - 1000), vec3(0, 0, camera.position.z + 1000), mu::Z);
    }
    for (int x = 0; x <= room->getTileMap().size.x; x++)
    {
        lineRenderer.line(vec3(x, 0, 0), vec3(x, 0, room->getTileMap().size.z), mu::ONE_3);
    }
    for (int z = 0; z <= room->getTileMap().size.z; z++)
    {
        lineRenderer.line(vec3(0, 0, z), vec3(room->getTileMap().size.x, 0, z), mu::ONE_3);
    }
}

IsoRoomScreen::~IsoRoomScreen()
{

}

void IsoRoomScreen::showTerrainEditor()
{
    static IsoTileShape shape = IsoTileShape::full;
    static uint yLevel = 0u;
    static uint rotation = 0u;

    ImGui::SetNextWindowSize(ImVec2(250.0f, -1.0f), ImGuiCond_Appearing);
    if (ImGui::Begin("Terrain"))
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(255, 0, 0, 100));
        ImGui::BeginChild("Shape", ImVec2(-1, 80), true, ImGuiWindowFlags_None);
        {
            ImGui::Text("Tile shape:");
            static const ImVec2 btnSize(40, 40);
            if (ImGui::Button("Empty", btnSize))
            {
                shape = IsoTileShape::empty;
            }
            ImGui::SameLine();
            if (ImGui::Button("Full", btnSize))
            {
                shape = IsoTileShape::full;
            }
            ImGui::SameLine();
            if (ImGui::Button("Slope", btnSize))
            {
                shape = IsoTileShape::slope;
            }
            ImGui::SameLine();
            if (ImGui::Button("Slope corner", btnSize))
            {
                shape = IsoTileShape::slope_corner;
            }
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 100, 255, 100));
        ImGui::BeginChild("Layer", ImVec2(-1, 80), true, ImGuiWindowFlags_None);
        {
            ImGui::Text("Tile layer:");
            static const uint step = 1;
            ImGui::InputScalar("", ImGuiDataType_U32, &yLevel, &step, &step, "%u");
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 255, 100, 100));
        ImGui::BeginChild("Rotation", ImVec2(-1, 80), true, ImGuiWindowFlags_None);
        {
            ImGui::Text("Rotation:");

            if (UIScreenWidgets::modelSpriteImageButton("tilerotleft", "RotateArrow", false, true))
            {
                if (rotation == 0u)
                {
                    rotation = 3u;
                }
                else
                {
                    rotation--;
                }
            }
            ImGui::SameLine();
            if (UIScreenWidgets::modelSpriteImageButton("tilerotright", "RotateArrow", false, false))
            {
                if (rotation == 3u)
                {
                    rotation = 0u;
                }
                else
                {
                    rotation++;
                }
            }
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();
    }
    ImGui::End();

    if (MouseInput::yScroll > 0.0)
    {
        yLevel++;
    }
    if (MouseInput::yScroll < 0.0 && yLevel > 0u)
    {
        yLevel--;
    }
    if (yLevel >= room->getTileMap().size.y)
    {
        yLevel = 0u;
    }

    if (gu::widthPixels == 0 || gu::heightPixels == 0 || camera.direction.y == 0.0f)
    {
        return;
    }

    vec3 cursorPos3d = camera.position;
    cursorPos3d -= camera.right * camera.viewportWidth * 0.5f;
    cursorPos3d += camera.right * float(MouseInput::mouseX / gu::widthPixels) * camera.viewportWidth;

    cursorPos3d += camera.up * camera.viewportHeight * 0.5f;
    cursorPos3d -= camera.up * float(MouseInput::mouseY / gu::heightPixels) * camera.viewportHeight;

    float deltaY = cursorPos3d.y - float(yLevel) * IsoTileMap::TILE_HEIGHT;
    vec3 ray = camera.direction;
    ray *= -1.0 / ray.y;

    cursorPos3d += ray * deltaY;

    if (mu::allGreaterOrEqualTo(cursorPos3d, 0.0))
    {
        uvec3 tilePos = uvec3(cursorPos3d.x, cursorPos3d.y / IsoTileMap::TILE_HEIGHT, cursorPos3d.z);

        auto &map = room->getTileMap();
        if (map.isValidPosition(tilePos.x, tilePos.y, tilePos.z))
        {
            lineRenderer.square(
                vec3(tilePos.x, tilePos.y * IsoTileMap::TILE_HEIGHT, tilePos.z),
                1.0f, mu::ONE_3, mu::X, mu::Z
            );

            const IsoTile &currentTile = map.getTile(tilePos.x, tilePos.y, tilePos.z);
            const IsoTileMaterial &tileMaterial = IsoTileMap::getMaterial(currentTile.material);

            ImGui::SetTooltip("%hhu %s", currentTile.shape, tileMaterial.name.c_str());

            if (MouseInput::justPressed(GLFW_MOUSE_BUTTON_LEFT))
            {
                map.setTile(tilePos.x, tilePos.y, tilePos.z, { shape, rotation });
            }
            else if (MouseInput::justPressed(GLFW_MOUSE_BUTTON_RIGHT))
            {
                map.setTile(tilePos.x, tilePos.y, tilePos.z, { IsoTileShape::empty, 0u });
            }
        }
    }
}
