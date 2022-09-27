
#include "ModelSpriteRenderer.h"

#include "../../game/Game.h"
#include "../../level/room3d/Room3D.h"

#include <graphics/3d/vert_buffer.h>

ModelSpriteRenderer::ModelSpriteRenderer()
    :
    shader("Model sprite shader", "shaders/iso/model_sprite.vert", "shaders/iso/model_sprite.frag"),
    instancedData(
        VertAttributes()
            .add_({"POS", 3})
            .add_({"SIZE", 2})
            .add_({"TEXTURE_OFFSET", 2}),
        std::vector<u_char>()
    )
{
    quad = Mesh::createQuad(-1.0f, 1.0f);
    VertBuffer::uploadSingleMesh(quad);
    quad->vertBuffer->vboUsage = GL_DYNAMIC_DRAW;
}

void ModelSpriteRenderer::add(const ModelSpriteView &spriteView, const Transform &transform, const Camera &camera)
{
    const SharedModelSprite &sprite = Game::spriteSheet->getModelSpriteByName(spriteView.modelName);

    mat4 transMatrix = Room3D::transformFromComponent(transform);

    vec3 direction = transMatrix * vec4(mu::X, 0.0f);

    float yaw = float(atan2(direction.z, direction.x)) * mu::RAD_TO_DEGREES;
    yaw -= float(atan2(camera.direction.z, camera.direction.x)) * mu::RAD_TO_DEGREES;

    const ModelSprite::Orientation &orientation = sprite->getClosestOrientation(yaw, 0.0f);
    const auto animationIt = orientation.animations.find(spriteView.animation);

    if (animationIt == orientation.animations.end())
    {
        return;
    }

    const std::vector<ModelSprite::Orientation::Frame> &animation = animationIt->second;

    if (animation.empty())
    {
        return;
    }

    uint frameI = clamp<uint>(spriteView.frame, 0, animation.size());

    int i = instancedData.nrOfVertices();
    instancedData.addVertices(1);
    int attrOffset = 0;
    instancedData.set<vec3>(transform.position, i, attrOffset);
    attrOffset += sizeof(vec3);
    instancedData.set<vec2>(sprite->renderProperties.spriteSize, i, attrOffset);
    attrOffset += sizeof(vec2);
    instancedData.set<vec2>(vec2(animation[frameI].spriteSheetOffset), i, attrOffset);
    attrOffset += sizeof(vec2);
}

void ModelSpriteRenderer::render(const Camera &camera)
{
    instancedDataID = quad->vertBuffer->uploadPerInstanceData(instancedData, 1, instancedDataID);

    shader.use();
    glUniformMatrix4fv(shader.location("projection"), 1, GL_FALSE, &camera.combined[0][0]);
    glUniform3fv(shader.location("cameraUpDirection"), 1, &camera.up[0]);
    glUniform3fv(shader.location("cameraRightDirection"), 1, &camera.right[0]);
    Game::spriteSheet->fbo->colorTexture->bind(0, shader, "spriteSheet");
    Game::spriteSheet->fbo->depthTexture->bind(1, shader, "spriteSheetDepth");

    quad->renderInstances(instancedData.nrOfVertices());
    instancedData.vertices.clear();
}
