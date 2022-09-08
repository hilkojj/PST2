
#ifndef GAME_LEVELSCREEN_H
#define GAME_LEVELSCREEN_H

#include <gu/screen.h>
#include <level/Level.h>
#include "room3d/Room3DScreen.h"
#include "isometric_room/IsoRoomScreen.h"

class LevelScreen : public Screen
{

    Level *lvl;

    Room3DScreen *room3DScreen = NULL;
    IsoRoomScreen *isoRoomScreen = NULL;

    delegate_method onPlayerEnteredRoom, onRoomDeletion;

  public:

    LevelScreen(Level *);

    void render(double deltaTime) override;

    void onResize() override;

    void showRoom(Room *);

  private:

    void renderDebugTools();

};


#endif
