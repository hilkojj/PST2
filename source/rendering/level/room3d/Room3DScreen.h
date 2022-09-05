
#ifndef GAME_ROOM3DSCREEN_H
#define GAME_ROOM3DSCREEN_H

#include <graphics/frame_buffer.h>
#include "EntityInspector3D.h"
#include "Room3DRenderer.h"

struct RenderModel;
struct Rigged;

class Room3DScreen : public Screen
{

    Room3DRenderer renderer;
    EntityInspector3D inspector;
    GizmoRenderer gizmoRenderer;

    DebugLineRenderer lineRenderer;

    ShaderAsset
        defaultShader, riggedShader, skyShader;

  public:
      
    Room3D *room;

    Room3DScreen(Room3D *room);

    void render(double deltaTime) override;

    void onResize() override;

    void renderDebugStuff(double deltaTime);

    ~Room3DScreen() override;

  private:

    // debug:
    void debugText(const std::string &, const vec3 &);

    void debugLights();

    void debugArmatures();
};


#endif
