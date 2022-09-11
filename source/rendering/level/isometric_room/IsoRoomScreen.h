
#ifndef GAME_ISOROOMSCREEN_H
#define GAME_ISOROOMSCREEN_H

#include "IsoTileMapMeshGenerator.h"
#include "IsoEntityInspector.h"
#include "../../../level/isometric_room/IsoRoom.h"

#include <graphics/orthographic_camera.h>
#include <graphics/frame_buffer.h>
#include <graphics/3d/debug_line_renderer.h>
#include <graphics/shader_asset.h>

class IsoRoomScreen : public Screen
{
    DebugLineRenderer lineRenderer;

    IsoEntityInspector entityInspector;

    IsoTileMapMeshGenerator tileMapMeshGenerator;

    ShaderAsset tileMapShader;

    OrthographicCamera camera;

  public:

    IsoRoom *room;

    IsoRoomScreen(IsoRoom *room);

    void render(double deltaTime) override;

    void onResize() override;

    void showTerrainEditor();

    void renderDebugStuff(double deltaTime);

    ~IsoRoomScreen() override;

};


#endif //GAME_ISOROOMSCREEN_H
