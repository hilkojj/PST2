
#ifndef GAME_ROOM3DRENDERER_H
#define GAME_ROOM3DRENDERER_H

#include <gu/screen.h>
#include <gl_includes.h>
#include <graphics/3d/debug_line_renderer.h>
#include <graphics/camera.h>
#include <graphics/shader_asset.h>
#include <graphics/cube_map.h>
#include "../../../level/room3d/Room3D.h"
#include "../../../generated/Model.hpp"

class Room3DRenderer
{
    Texture dummyTexture;
    Room3D *room;

  public:
    struct Context
    {
        Camera &cam;
        ShaderProgram &shader, &riggedShader;
        double time = 0.0;
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

    Room3DRenderer(Room3D *room);

    void renderRoom(const Context &);

    void initializeShader(const Context &, ShaderProgram &);

    void prepareMaterial(entt::entity, const Context &, const ModelPart &, ShaderProgram &);

    void renderModel(const Context &, ShaderProgram &, entt::entity, const Transform &, const RenderModel &, const Rigged *rig=NULL);

};


#endif //GAME_ROOM3DRENDERER_H
