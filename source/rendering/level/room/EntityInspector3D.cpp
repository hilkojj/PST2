
#include "EntityInspector3D.h"
#include "../../../generated/Transform.hpp"
#include <generated/Inspecting.hpp>
#include <input/key_input.h>
#include <btBulletCollisionCommon.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>

void EntityInspector3D::drawGUI(const Camera *cam, DebugLineRenderer &dlr, GizmoRenderer &gizmoRenderer)
{
    EntityInspector::drawGUI(cam, dlr);

    if (movingEntity != entt::null && engine.entities.valid(movingEntity))
    {
        if (auto t = engine.entities.try_get<Transform>(movingEntity))
        {
            if (gizmoRenderer.gizmo(("entity_gizmo_" + std::to_string(int(movingEntity))).c_str(), t->position, t->rotation, t->scale))
            {

            }
        }
    }
}

void EntityInspector3D::pickEntityGUI(const Camera *cam, DebugLineRenderer &renderer)
{
    pickEntity = false;

    if (KeyInput::justPressed(GLFW_KEY_ESCAPE))
        return;

    pickEntity = true;

    auto toInspect = pickEntityInRoom(cam, renderer);

    if (toInspect != entt::null)
    {
        MouseInput::capture(GLFW_MOUSE_BUTTON_LEFT, 10, 10);
        if (MouseInput::justPressed(GLFW_MOUSE_BUTTON_LEFT, 10))
        {
            reg.assign_or_replace<Inspecting>(toInspect);
            pickEntity = false;
        }
    }
}

entt::entity EntityInspector3D::pickEntityInRoom(const Camera *cam, DebugLineRenderer &renderer, bool preferBody)
{
    return entt::null;
}

void EntityInspector3D::moveEntityGUI(const Camera *cam, DebugLineRenderer &renderer)
{
    if (KeyInput::justPressed(GLFW_KEY_ESCAPE))
    {
        moveEntity = false;
        movingEntity = entt::null;
        return;
    }
    if (KeyInput::justPressed(GLFW_KEY_ENTER))
    {
        moveEntity = false;
        return;
    }
    moveEntity = true;
    //movingEntity = entt::null;

    auto toInspect = pickEntityInRoom(cam, renderer, true);

    if (toInspect != entt::null)
    {
        MouseInput::capture(GLFW_MOUSE_BUTTON_LEFT, 10, 10);
        if (MouseInput::justPressed(GLFW_MOUSE_BUTTON_LEFT, 10))
        {
            movingEntity = toInspect;
            moveEntity = false;
        }
    }
}

void EntityInspector3D::highLightEntity(entt::entity entity, const Camera *camera, DebugLineRenderer &renderer)
{
}
