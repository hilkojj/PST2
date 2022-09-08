
#ifndef GAME_ISOENTITYINSPECTOR_H
#define GAME_ISOENTITYINSPECTOR_H

#include <ecs/EntityInspector.h>

class IsoEntityInspector : public EntityInspector
{
  public:
    using EntityInspector::EntityInspector;

  protected:
    void pickEntityGUI(const Camera *camera, DebugLineRenderer &renderer) override;

    void moveEntityGUI(const Camera *camera, DebugLineRenderer &renderer) override;

    void highLightEntity(entt::entity entity, const Camera *camera, DebugLineRenderer &renderer) override;

};


#endif //GAME_ISOENTITYINSPECTOR_H
