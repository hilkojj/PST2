
#ifndef GAME_MODELSPRITERENDERER_H
#define GAME_MODELSPRITERENDERER_H

#include <graphics/3d/mesh.h>
#include <graphics/camera.h>
#include <graphics/shader_asset.h>
#include "../../../external/entt/src/entt/entity/registry.hpp"
#include "../../generated/ModelSpriteView.hpp"
#include "../../generated/Transform.hpp"

class ModelSpriteRenderer
{
    SharedMesh quad;
    VertData instancedData;
    int instancedDataID = -1;

    ShaderAsset shader;

  public:
    ModelSpriteRenderer();

    void add(const ModelSpriteView &, const Transform &, const Camera &);

    void render(const Camera &);
};


#endif //GAME_MODELSPRITERENDERER_H
