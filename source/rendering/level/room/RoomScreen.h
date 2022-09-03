
#ifndef GAME_ROOMSCREEN_H
#define GAME_ROOMSCREEN_H

#include <gu/screen.h>
#include <gl_includes.h>
#include <graphics/3d/debug_line_renderer.h>
#include <graphics/camera.h>
#include <graphics/shader_asset.h>
#include <graphics/frame_buffer.h>
#include <graphics/cube_map.h>
#include "../../../level/room/Room3D.h"

struct RenderModel;
struct Rigged;

class RoomScreen : public Screen
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
            shadows = true,
            materials = true,
            riggedModels = true,
            customShaders = true;
        ShaderProgram *skyShader = NULL;
        CubeMap *skyBox = NULL;
    };

    DebugLineRenderer lineRenderer;

    ShaderAsset
        defaultShader, riggedShader, skyShader;

    int prevNrOfPointLights = -1, prevNrOfDirLights = -1, prevNrOfDirShadowLights = -1;

    FrameBuffer *fbo = NULL;

    Texture dummyTexture;

  public:
      
    Room3D *room;

    RoomScreen(Room3D *room);

    void render(double deltaTime) override;

    void onResize() override;

    void renderDebugStuff(double deltaTime);

    ~RoomScreen() override;

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
