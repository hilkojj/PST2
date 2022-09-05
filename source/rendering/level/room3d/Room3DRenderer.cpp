
#include "Room3DRenderer.h"

#include <game/dibidab.h>
#include "../../../generated/Light.hpp"
#include "../../../game/Game.h"
#include "../../../ecs/systems/graphics/3d/CustomShaderSystem.h"

#define DISPOSE_VERTS_ON_UPLOAD true

const GLubyte dummyTexData[] = {0, 0, 0};

int prevNrOfPointLights = -1, prevNrOfDirLights = -1, prevNrOfDirShadowLights = -1;

Room3DRenderer::Room3DRenderer(Room3D *room)
: room(room),
  dummyTexture(Texture::fromByteData(&dummyTexData[0], GL_RGB, 1, 1, GL_NEAREST, GL_NEAREST))
{

}

void Room3DRenderer::renderRoom(const Room3DRenderer::Context &con)
{
    gu::profiler::Zone z("render models");

    initializeShader(con, con.shader);

    room->entities.view<Transform, RenderModel>(entt::exclude<Rigged>).each([&](auto e, Transform &t, RenderModel &rm) {
        if (con.filter && !con.filter(e)) return;
        if (con.customShaders && room->entities.has<CustomShader>(e)) return; // TODO: move to entt::exclude.
        renderModel(con, con.shader, e, t, rm);
    });

    if (con.customShaders)
    {
        auto &collections = CustomShaderSystem::getEntitiesPerShaderForRoom(room);
        for (auto &[shader, entities] : collections)
        {
            if (entities.empty())
                continue;

            initializeShader(con, *shader.get());

            for (auto e : entities)
            {
                Transform *t = room->entities.try_get<Transform>(e);
                RenderModel *rm = room->entities.try_get<RenderModel>(e);

                if (!t || !rm)
                    continue;

                if (CustomShader *cs = room->entities.try_get<CustomShader>(e))
                {
                    for (auto &[key, val] : cs->uniformsFloat)
                        glUniform1f(shader->location(key.c_str()), val);
                    for (auto &[key, val] : cs->uniformsInt)
                        glUniform1i(shader->location(key.c_str()), val);
                    for (auto &[key, val] : cs->uniformsVec2)
                        glUniform2f(shader->location(key.c_str()), val.x, val.y);
                    for (auto &[key, val] : cs->uniformsVec3)
                        glUniform3f(shader->location(key.c_str()), val.x, val.y, val.z);
                    for (auto &[key, val] : cs->uniformsVec4)
                        glUniform4f(shader->location(key.c_str()), val.x, val.y, val.z, val.w);
                }

                if (Rigged *rig = room->entities.try_get<Rigged>(e))
                {
                    renderModel(con, *shader.get(), e, *t, *rm, rig);
                }
                else
                {
                    renderModel(con, *shader.get(), e, *t, *rm);
                }
            }
        }
    }

    if (con.riggedModels)
    {
        gu::profiler::Zone z1("rigged models");

        bool first = true;
        room->entities.view<Transform, RenderModel, Rigged>().each([&](auto e, Transform &t, RenderModel &rm, Rigged &rig) {
            if (con.filter && !con.filter(e)) return;
            if (con.customShaders && room->entities.has<CustomShader>(e)) return; // TODO: move to entt::exclude.
            if (first)
            {
                initializeShader(con, con.riggedShader);
                first = false;
            }
            renderModel(con, con.riggedShader, e, t, rm, &rig);
        });
    }

    if (con.skyShader)
    {
        con.skyShader->use();

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        if (con.skyBox)
        {
            con.skyBox->bind(0);
            glUniform1i(con.skyShader->location("skyBox"), 0);

            glUniformMatrix4fv(con.skyShader->location("projection"), 1, GL_FALSE, &con.cam.projection[0][0]);
            glUniformMatrix4fv(con.skyShader->location("view"), 1, GL_FALSE, &con.cam.view[0][0]);

            Mesh::getCube()->render();
        }
        else
            Mesh::getQuad()->render();

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
    }
}

const int
        DIFFUSE_TEX_UNIT = 0,
        MET_ROUG_TEX_UNIT = 1,
        NORMAL_TEX_UNIT = 2;

const char
        *DIFFUSE_UNI_NAME = "diffuseTexture",
        *MET_ROUG_UNI_NAME = "metallicRoughnessTexture",
        *NORMAL_UNI_NAME = "normalMap";

void Room3DRenderer::prepareMaterial(entt::entity e, const Room3DRenderer::Context &con, const ModelPart &modelPart, ShaderProgram &shader)
{
    bool useDiffuseTexture = modelPart.material->diffuseTexture.isSet();
    glUniform1i(shader.location("useDiffuseTexture"), useDiffuseTexture);
    if (useDiffuseTexture)
        modelPart.material->diffuseTexture.get().bind(DIFFUSE_TEX_UNIT, shader, DIFFUSE_UNI_NAME);
    else
    {
        glUniform3fv(shader.location("diffuse"), 1, &modelPart.material->diffuse[0]);
        dummyTexture.bind(DIFFUSE_TEX_UNIT, shader, DIFFUSE_UNI_NAME);
    }

    bool useMetallicRoughnessTexture = modelPart.material->metallicRoughnessTexture.isSet();
    glUniform1i(shader.location("useMetallicRoughnessTexture"), useMetallicRoughnessTexture);
    if (useMetallicRoughnessTexture)
        modelPart.material->metallicRoughnessTexture.get().bind(MET_ROUG_TEX_UNIT, shader, MET_ROUG_UNI_NAME);
    else
    {
        glUniform2fv(shader.location("metallicRoughnessFactors"), 1, &modelPart.material->metallic);
        dummyTexture.bind(MET_ROUG_TEX_UNIT, shader, MET_ROUG_UNI_NAME);
    }

    bool useNormalMap = modelPart.material->normalMap.isSet();
    glUniform1i(shader.location("useNormalMap"), useNormalMap);
    if (useNormalMap)
        modelPart.material->normalMap.get().bind(NORMAL_TEX_UNIT, shader, NORMAL_UNI_NAME);
    else
        dummyTexture.bind(NORMAL_TEX_UNIT, shader, NORMAL_UNI_NAME);
}

inline void prepareMeshVertBuffer(const SharedMesh &mesh)
{
    if (!mesh->vertBuffer)
        throw gu_err("Mesh (" + mesh->name + ") is not assigned to any VertBuffer!");

    if (!mesh->vertBuffer->isUploaded())
        mesh->vertBuffer->upload(DISPOSE_VERTS_ON_UPLOAD);
}

void Room3DRenderer::renderModel(const Room3DRenderer::Context &con, ShaderProgram &shader, entt::entity e, const Transform &t, const RenderModel &rm, const Rigged *rig)
{
    if (!(rm.visibilityMask & con.mask))
        return;

    auto &model = room->models[rm.modelName];
    if (!model)
        return;

    auto transform = Room3D::transformFromComponent(t);

    mat4 mvp = con.cam.combined * transform;
    glUniformMatrix4fv(shader.location("mvp"), 1, GL_FALSE, &mvp[0][0]);
    if (con.materials)
        glUniformMatrix4fv(shader.location("transform"), 1, GL_FALSE, &transform[0][0]);

    for (auto &modelPart : model->parts)
    {
        if (!modelPart.material || !modelPart.mesh) // TODO: give warning if modelpart has no material?
            continue;

        prepareMeshVertBuffer(modelPart.mesh);

        if (con.materials)
            prepareMaterial(e, con, modelPart, shader);

        if (rig && !modelPart.armature->bones.empty())
        {
            std::vector<mat4> matrices(modelPart.armature->bones.size());
            int i = 0;
            for (auto &bone : modelPart.armature->bones)
            {
                auto it = rig->bonePoseTransform.find(bone);
                matrices[i++] = it == rig->bonePoseTransform.end() ? mat4(1) : it->second;
            }
            glUniformMatrix4fv(shader.location("bonePoseTransforms"), matrices.size(), false, &matrices[0][0][0]);
        }

        modelPart.mesh->render(modelPart.meshPartIndex);
    }
}

void Room3DRenderer::initializeShader(const Room3DRenderer::Context &con, ShaderProgram &shader)
{
    if (con.lights && con.uploadLightData && !shader.definitions.isDefined("NO_LIGHTS"))
    {
        auto plView = room->entities.view<Transform, PointLight>();
        int nrOfPointLights = plView.size();
        if (nrOfPointLights != prevNrOfPointLights)
        {
            ShaderDefinitions::global().defineInt("NR_OF_POINT_LIGHTS", nrOfPointLights);
            prevNrOfPointLights = nrOfPointLights;
        }

        auto dlView = room->entities.view<Transform, DirectionalLight>();
        int nrOfDirLights = dlView.size();
        if (nrOfDirLights != prevNrOfDirLights)
        {
            ShaderDefinitions::global().defineInt("NR_OF_DIR_LIGHTS", nrOfDirLights);
            prevNrOfDirLights = nrOfDirLights;
        }

        shader.use();

        int pointLightI = 0;
        plView.each([&](Transform &t, PointLight &pl) {

            std::string arrEl = "pointLights[" + std::to_string(pointLightI++) + "]";

            glUniform3fv(shader.location((arrEl + ".position").c_str()), 1, &t.position[0]);
            glUniform3fv(shader.location((arrEl + ".attenuation").c_str()), 1, &vec3(pl.constant, pl.linear, pl.quadratic)[0]); // todo, point to pl.constant?
            glUniform3fv(shader.location((arrEl + ".color").c_str()), 1, &pl.color[0]);
        });

        int dirLightI = 0;
        dlView.each([&](auto e, Transform &t, DirectionalLight &dl) {

            std::string arrEl = "dirLights[" + std::to_string(dirLightI++) + "]";

            auto transform = Room3D::transformFromComponent(t);
            vec3 direction = transform * vec4(-mu::Y, 0);

            glUniform3fv(shader.location((arrEl + ".direction").c_str()), 1, &direction[0]);
            glUniform3fv(shader.location((arrEl + ".color").c_str()), 1, &dl.color[0]);
        });
    }
    else shader.use();

    glUniform3fv(shader.location("camPosition"), 1, &con.cam.position[0]);
    glUniform1f(shader.location("time"), con.time);
}
