#include <asset_manager/asset.h>
#include <game/dibidab.h>
#include <utils/aseprite/AsepriteLoader.h>

#include "game/Game.h"
#include "rendering/GameScreen.h"


float AsepriteView::playTag(const char *tagName, bool unpause)
{
    int i = 0;
    for (auto &tag : sprite->tags)
    {
        if (tag.name == tagName)
        {
            frame = tag.from;
            frameTimer = 0;
            pong = false;
            playingTag = i;
            paused = unpause ? false : paused;
            return tag.duration;
        }
        i++;
    }
    return 0;
}

void addAssetLoaders()
{
    // more loaders are added in dibidab::init()

    AssetManager::addAssetLoader<aseprite::Sprite>(".ase", [&](auto path) {

        auto sprite = new aseprite::Sprite;
        aseprite::Loader(path.c_str(), *sprite);
        Game::spriteSheet.add(*sprite);
        return sprite;
    });
}

void initLuaStuff()
{
    auto &env = luau::getLuaState();

    env["gameEngineSettings"] = dibidab::settings;
    env["gameSettings"] = Game::settings;
    env["saveGameSettings"] = [] {
        Game::saveSettings();
    };

    // sprite function: // todo: maybe this should not be in main.cpp
    env["playAsepriteTag"] = [] (AsepriteView &view, const char *tagName, sol::optional<bool> unpause) -> float {
        return view.playTag(tagName, unpause.value_or(false));
    };
    env["asepriteTagFrames"] = [] (AsepriteView &view, const char *tagName) {
        assert(view.sprite.isSet());
        auto &tag = view.sprite->getTagByName(tagName);
        return std::make_tuple(tag.from, tag.to);
    };
}

int main(int argc, char *argv[])
{
    addAssetLoaders();
    Game::loadSettings();

    Level::customRoomLoader = [] (const json &j) {
        auto *room = new Room3D;
        room->fromJson(j);
        return room;
    };

    dibidab::init(argc, argv);

    File::createDir("./saves"); // todo, see dibidab trello
    gu::setScreen(new GameScreen);

    initLuaStuff();

    Game::uiScreenManager->openScreen(asset<luau::Script>("scripts/ui_screens/StartupScreen"));

    Game::spriteSheet.printUsage();

    dibidab::run();

    Game::saveSettings();

    #ifdef EMSCRIPTEN
    return 0;
    #else
    quick_exit(0);
    #endif
}
