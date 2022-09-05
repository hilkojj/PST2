#ifndef GAME_ISOMETRICSPRITESGENERATOR_H
#define GAME_ISOMETRICSPRITESGENERATOR_H

#include "../../level/room3d/Room3D.h"
#include "../level/room3d/Room3DRenderer.h"

class IsometricSpritesGenerator
{

    Room3D room3D;
    Room3DRenderer renderer;
    ShaderAsset defaultShader, riggedShader;
    std::vector<std::string> entitiesToRender;
    uint totalNumEntities;

  public:

    std::string currentlyLoading;

    IsometricSpritesGenerator();

    /*
     * returns progress
     */
    float generate();
};

#endif //GAME_ISOMETRICSPRITESGENERATOR_H
