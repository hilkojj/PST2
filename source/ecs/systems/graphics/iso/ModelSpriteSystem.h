
#ifndef GAME_MODELSPRITESYSTEM_H
#define GAME_MODELSPRITESYSTEM_H

#include <ecs/systems/EntitySystem.h>

class ModelSpriteSystem : public EntitySystem
{
    using EntitySystem::EntitySystem;

  protected:

    void update(double deltaTime, EntityEngine *room) override;

};

#endif //GAME_MODELSPRITESYSTEM_H
