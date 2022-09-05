#ifndef GAME_MODELSPRITE_H
#define GAME_MODELSPRITE_H

#include "../../generated/Model.hpp"

#include <memory>

class ModelSprite;

typedef std::shared_ptr<ModelSprite> SharedModelSprite;

class ModelSprite
{
  public:
    struct Orientation
    {
        struct Frame
        {
            ivec2 spriteSheetOffset;
        };

        std::map<std::string, std::vector<Frame>> animations;
    };

    const RenderModelToSpriteSheet renderProperties;

    ModelSprite(const RenderModelToSpriteSheet &);

    Orientation &getClosestOrientation(const float yaw, const float pitch);

  private:

    std::vector<std::vector<Orientation>> orientations; // [yawStep][pitchStep]
};


#endif //GAME_MODELSPRITE_H
