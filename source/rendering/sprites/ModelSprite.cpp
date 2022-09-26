
#include "ModelSprite.h"

ModelSprite::ModelSprite(const RenderModelToSpriteSheet &renderProperties) : renderProperties(renderProperties)
{
    orientations.resize(renderProperties.yawSteps);
    for (uint yawStep = 0; yawStep < renderProperties.yawSteps; yawStep++)
    {
        orientations[yawStep].resize(renderProperties.pitchSteps);
    }
}

ModelSprite::Orientation &ModelSprite::getClosestOrientation(float yaw, float pitch)
{
    if (yaw < 0.0f)
    {
        yaw = ceil(-yaw / 360.0f) * 360 + yaw;
    }
    uint yawIndex = uint(round((yaw / 360.0f) * float(renderProperties.yawSteps))) % renderProperties.yawSteps;

    uint pitchIndex = 0;// todo

    return orientations[yawIndex][pitchIndex];
}
