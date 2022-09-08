#ifndef GAME_ISOROOM_H
#define GAME_ISOROOM_H

#include "IsoTileMap.h"

#include <level/room/Room.h>

class IsoRoom : public Room
{
  public:

    IsoRoom();

    explicit IsoRoom(const uvec3 &tileMapSize);

    void initialize(Level *lvl) override;

    void update(double deltaTime) override;

    IsoTileMap &getTileMap();

    const IsoTileMap &getTileMap() const;

    void toJson(json &) override;

    void fromJson(const json &) override;

    ~IsoRoom() override;

    vec3 cameraFocus;
    vec3 cameraDirection;

  protected:
    void initializeLuaEnvironment() override;

  private:

    void updateCamera(double deltaTime);

    IsoTileMap *tileMap = nullptr;

};

#endif //GAME_ISOROOM_H
