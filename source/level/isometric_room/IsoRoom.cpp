
#include "IsoRoom.h"

#include "../../game/Game.h"

#include <level/Level.h>

#include <utils/base64.h>

IsoRoom::IsoRoom() : cameraFocus(0), cameraDirection(mu::X + mu::Z)
{
    templateFolder = "scripts/entities/simulation/";
}

IsoRoom::IsoRoom(const uvec3 &tileMapSize) : IsoRoom()
{
    tileMap = new IsoTileMap(tileMapSize);
}

void IsoRoom::initialize(Level *lvl)
{
    assert(tileMap);

    // add systems

    Room::initialize(lvl);
}

void IsoRoom::update(double deltaTime)
{
    Room::update(deltaTime);

    updateCamera(deltaTime);
}

void IsoRoom::initializeLuaEnvironment()
{
    Room::initializeLuaEnvironment();

    luaEnvironment["currentRoom"] = luaEnvironment;
    luaEnvironment["getTime"] = [&] {
        return getLevel().getTime();
    };
}

void IsoRoom::toJson(json &j)
{
    assert(tileMap);
    Room::toJson(j);
    j["tileMapSize"] = tileMap->size;
    std::vector<char> tileMapBinary;
    tileMap->toBinary(tileMapBinary);
    std::string tileMapBase64 = base64::encode(&tileMapBinary[0], tileMapBinary.size());
    j["tileMapBase64"] = tileMapBase64;
    j["cameraFocus"] = cameraFocus;
}

void IsoRoom::fromJson(const json &j)
{
    assert(tileMap == nullptr);
    Room::fromJson(j);
    std::string tileMapBase64 = j.at("tileMapBase64");
    auto tileMapBinary = base64::decode(&tileMapBase64[0], tileMapBase64.size());
    uvec3 tileMapSize = j.at("tileMapSize");
    tileMap = new IsoTileMap(tileMapSize);
    tileMap->fromBinary(&tileMapBinary[0], tileMapBinary.size());
    cameraFocus = j.at("cameraFocus");
}

IsoTileMap &IsoRoom::getTileMap()
{
    if (!tileMap)
    {
        throw gu_err("IsoRoom has no tileMap.");
    }
    return *tileMap;
}

const IsoTileMap &IsoRoom::getTileMap() const
{
    if (!tileMap)
    {
        throw gu_err("IsoRoom has no tileMap.");
    }
    return *tileMap;
}

IsoRoom::~IsoRoom()
{
    delete tileMap;
}

void IsoRoom::updateCamera(double deltaTime)
{
    static float cameraMoveSpeedMultiplier = 1.0f;

    const auto prevCameraFocus = cameraFocus;
    const float moveAmount = Game::settings.keyInput.moveCameraSpeed * cameraMoveSpeedMultiplier * deltaTime;

    if (KeyInput::pressed(Game::settings.keyInput.moveCameraUp))
    {
        cameraFocus += normalize(cameraDirection) * moveAmount;
    }
    if (KeyInput::pressed(Game::settings.keyInput.moveCameraDown))
    {
        cameraFocus -= normalize(cameraDirection) * moveAmount;
    }
    if (KeyInput::pressed(Game::settings.keyInput.moveCameraLeft))
    {
        vec3 leftDirection = rotate(cameraDirection, 90 * mu::DEGREES_TO_RAD, mu::Y);
        cameraFocus += normalize(leftDirection) * moveAmount;
    }
    if (KeyInput::pressed(Game::settings.keyInput.moveCameraRight))
    {
        vec3 leftDirection = rotate(cameraDirection, 90 * mu::DEGREES_TO_RAD, mu::Y);
        cameraFocus -= normalize(leftDirection) * moveAmount;
    }

    if (cameraFocus != prevCameraFocus)
    {
        cameraMoveSpeedMultiplier = clamp<float>(cameraMoveSpeedMultiplier + deltaTime * 4.0f, 1.0f, 8.0f);
    }
    else
    {
        cameraMoveSpeedMultiplier = clamp<float>(cameraMoveSpeedMultiplier - deltaTime * 16.0f, 1.0f, 8.0f);
    }

    if (KeyInput::justPressed(Game::settings.keyInput.rotateCameraLeft))
    {
        cameraDirection = rotate(cameraDirection, -45.0f * mu::DEGREES_TO_RAD, mu::Y);
    }
    if (KeyInput::justPressed(Game::settings.keyInput.rotateCameraRight))
    {
        cameraDirection = rotate(cameraDirection, 45.0f * mu::DEGREES_TO_RAD, mu::Y);
    }
}
