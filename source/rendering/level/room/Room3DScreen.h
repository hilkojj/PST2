
#ifndef GAME_ROOM3DSCREEN_H
#define GAME_ROOM3DSCREEN_H

#include <gu/screen.h>
#include <gl_includes.h>
#include <graphics/3d/debug_line_renderer.h>
#include <graphics/camera.h>
#include <graphics/shader_asset.h>
#include <graphics/frame_buffer.h>
#include <graphics/cube_map.h>
#include "../../../level/room/Room3D.h"
#include "EntityInspector3D.h"

struct RenderModel;
struct Rigged;

class Room3DScreen : public Screen
{
    struct RenderContext
    {
        Camera &cam;
        ShaderProgram &shader, &riggedShader;
        uint mask;
        std::function<bool(entt::entity)> filter;
        bool
            lights = true,
            uploadLightData = true,
            materials = true,
            riggedModels = true,
            customShaders = true;
        ShaderProgram *skyShader = NULL;
        CubeMap *skyBox = NULL;
    };

    EntityInspector3D inspector;
    GizmoRenderer gizmoRenderer;

    DebugLineRenderer lineRenderer;

    ShaderAsset
        defaultShader, riggedShader, skyShader;

    int prevNrOfPointLights = -1, prevNrOfDirLights = -1, prevNrOfDirShadowLights = -1;

    Texture dummyTexture;

  public:
      
    Room3D *room;

    Room3DScreen(Room3D *room);

    void render(double deltaTime) override;

    void onResize() override;

    void renderDebugStuff(double deltaTime);

    ~Room3DScreen() override;

  private:

    void renderRoom(const RenderContext &);

    void initializeShader(const RenderContext &, ShaderProgram &);

    void prepareMaterial(entt::entity, const RenderContext &, const ModelPart &, ShaderProgram &);

    void renderModel(const RenderContext &, ShaderProgram &, entt::entity, const Transform &, const RenderModel &, const Rigged *rig=NULL);

    // debug:
    void debugText(const std::string &, const vec3 &);

    void debugLights();

    void debugArmatures();
};


#endif
