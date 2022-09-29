
#include "ModelSpriteSystem.h"

#include "../../../../generated/ModelSpriteView.hpp"
#include "../../../../level/isometric_room/IsoRoom.h"
#include "../../../../rendering/sprites/ModelSprite.h"
#include "../../../../game/Game.h"


void ModelSpriteSystem::update(double deltaTime, EntityEngine *room)
{
    std::set<entt::entity> animationsFinishedFor;
    room->entities.view<ModelSpriteView>().each([&](entt::entity e, ModelSpriteView &view) {

        const SharedModelSprite &sprite = Game::spriteSheet->getModelSpriteByName(view.modelName);

        // TODO: dont use getClosestOrientation here. Unnecessary calculations
        const ModelSprite::Orientation &orientation = sprite->getClosestOrientation(0.0f, 0.0f);
        const auto animationIt = orientation.animations.find(view.animation);

        if (animationIt == orientation.animations.end())
        {
            return;
        }

        const std::vector<ModelSprite::Orientation::Frame> &animation = animationIt->second;

        if (animation.empty())
        {
            return;
        }

        view.frameTimer += float(deltaTime) * view.speedMultiplier;
        const float frameTime = 1.0f / max(1.0f, float(sprite->renderProperties.fps));
        while (view.frameTimer >= frameTime)
        {
            view.frameTimer -= frameTime;
            view.frame++;
            if (view.frame >= animation.size())
            {
                if (view.loop)
                {
                    view.frame = 0u;
                }
                else
                {
                    animationsFinishedFor.insert(e);
                    view.frame = animation.size() - 1;
                }
            }
        }
    });

    for (entt::entity e : animationsFinishedFor)
    {
        if (room->entities.valid(e))
        {
            room->emitEntityEvent(e, true, "ModelSpriteAnimationFinished");
        }
    }
}
