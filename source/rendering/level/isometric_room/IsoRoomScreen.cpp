
#include "IsoRoomScreen.h"

#include "../../../game/Game.h"

#include <game/dibidab.h>

IsoRoomScreen::IsoRoomScreen(IsoRoom *room) : room(room), entityInspector(*room, "IsoRoom"), camera(0.1f, 1000.f, 0.0f, 0.0f)
{
    entityInspector.createEntity_showSubFolder = "simulation";
}

void IsoRoomScreen::render(double deltaTime)
{
    // set opengl settings and clear canvas
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // update camera
    camera.position = -normalize(room->cameraDirection) * 128.0f;
    vec3 camRightDir = normalize(cross(camera.position, mu::Y));

    camera.position = rotate(camera.position, Game::settings.graphics.isometricAngle * mu::DEGREES_TO_RAD, camRightDir);
    camera.lookAt(mu::ZERO_3);
    camera.position.x += room->cameraFocus.x;
    camera.position.z += room->cameraFocus.z;
    camera.update();

    renderDebugStuff(deltaTime);
}

void IsoRoomScreen::onResize()
{
    Screen::onResize();
    camera.viewportWidth = gu::widthPixels / Game::settings.graphics.pixelsPerMeter;
    camera.viewportHeight = gu::heightPixels / Game::settings.graphics.pixelsPerMeter;
    camera.update();
}

void IsoRoomScreen::renderDebugStuff(double deltaTime)
{
    if (!dibidab::settings.showDeveloperOptions)
        return;

    gu::profiler::Zone z("debug");
    lineRenderer.projection = camera.combined;

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
