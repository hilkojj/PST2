
#include "../level/isometric_room/IsoTileMap.h"

#include <game/dibidab.h>
#include "Game.h"

Game::Settings Game::settings;

#define SETTINGS_FILE_PATH "./settings.json"

void setShaderDefinitions()
{
    ShaderDefinitions::global().defineInt("MAX_BONES", Game::settings.graphics.maxArmatureBones);
    ShaderDefinitions::global().defineInt("PIXELS_PER_METER", Game::settings.graphics.pixelsPerMeter);
    ShaderDefinitions::global().defineFloat("TILE_HEIGHT", IsoTileMap::TILE_HEIGHT);
    #ifdef EMSCRIPTEN
    ShaderDefinitions::global().define("WEBGL");
    #endif
}

void Game::loadSettings()
{
    if (!File::exists(SETTINGS_FILE_PATH))
    {
        settings = Settings();
        std::cerr << "Settings file (" << SETTINGS_FILE_PATH << ") not found!" << std::endl;
        saveSettings();
        loadSettings();// hack for loading font
        return;
    }
    try {
        json j = json::parse(File::readString(SETTINGS_FILE_PATH));
        settings = j;
        dibidab::settings = j;
    }
    catch (std::exception &e)
    {
        settings = Settings();
        std::cerr << "Error while loading settings:\n" << e.what() << std::endl;
    }
    setShaderDefinitions();
}

void Game::saveSettings()
{
    setShaderDefinitions();
    json j = dibidab::settings;
    j.merge_patch(settings);

    File::writeBinary(SETTINGS_FILE_PATH, j.dump(4));
}

cofu<MegaSpriteSheet> Game::spriteSheet;

cofu<UIScreenManager> Game::uiScreenManager;
