#ifndef GAME_MODELSPRITESGENERATOR_H
#define GAME_MODELSPRITESGENERATOR_H

#include "../../level/room3d/Room3D.h"
#include "../level/room3d/Room3DRenderer.h"

class ModelSpritesGenerator
{

    Room3D room3D;
    Room3DRenderer renderer;
    ShaderAsset defaultShader, riggedShader;
    std::vector<std::string> entitiesToRender;
    uint totalNumEntities;

  public:

    std::string currentlyLoading;

    ModelSpritesGenerator();

    /*
     * returns progress
     */
    float generate();
};

#endif //GAME_MODELSPRITESGENERATOR_H
