
#ifndef GAME_LEVELSCREEN_H
#define GAME_LEVELSCREEN_H

#include <gu/screen.h>
#include <level/Level.h>
#include "room3d/Room3DScreen.h"

class LevelScreen : public Screen
{

    Level *lvl;

    Room3DScreen *roomScreen = NULL;

    delegate_method onPlayerEnteredRoom, onRoomDeletion;

  public:

    LevelScreen(Level *);

    void render(double deltaTime) override;

    void onResize() override;

    void showRoom(Room3D *);

  private:

    void renderDebugTools();

};


#endif
