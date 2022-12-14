
#ifndef GAME_GAMESCREEN_H
#define GAME_GAMESCREEN_H

#include <gu/screen.h>
#include "level/LevelScreen.h"
#include "sprites/ModelSpritesGenerator.h"

class GameScreen : public Screen
{
    LevelScreen *lvlScreen = NULL;

    ModelSpritesGenerator *spritesGenerator = NULL;

    delegate_method onSessionChange, onNewLevel;

  public:

    GameScreen();

    void render(double deltaTime) override;

    void onResize() override;

};


#endif
