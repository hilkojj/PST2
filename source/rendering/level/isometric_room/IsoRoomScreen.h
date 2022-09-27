
#ifndef GAME_ISOROOMSCREEN_H
#define GAME_ISOROOMSCREEN_H

#include "IsoTileMapMeshGenerator.h"
#include "IsoEntityInspector.h"
#include "../../sprites/ModelSpriteRenderer.h"
#include "../../../level/isometric_room/IsoRoom.h"

#include <graphics/orthographic_camera.h>
#include <graphics/frame_buffer.h>
#include <graphics/3d/debug_line_renderer.h>
#include <graphics/shader_asset.h>

class IsoRoomScreen : public Screen
{
  public:

    IsoRoom *room;

    IsoRoomScreen(IsoRoom *room);

    void render(double deltaTime) override;

    void onResize() override;

    ~IsoRoomScreen() override;

  private:

    void renderModelSprites();

    void showTerrainEditor();

    void renderDebugStuff(double deltaTime);

    bool findHoveredTile(uvec3 &outTilePos, vec3 &outHoverNormal);

    DebugLineRenderer lineRenderer;
    ModelSpriteRenderer modelSpriteRenderer;

    IsoEntityInspector entityInspector;

    IsoTileMapMeshGenerator tileMapMeshGenerator;

    ShaderAsset tileMapShader;

    OrthographicCamera camera;

    bool tileHovered = false;
    uvec3 hoveredTilePos;
    vec3 hoveredTileNormal;

};


#endif //GAME_ISOROOMSCREEN_H
