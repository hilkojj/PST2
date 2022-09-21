
#include "IsometricSpritesGenerator.h"
#include <graphics/orthographic_camera.h>
#include "../../generated/Model.hpp"
#include "../../game/Game.h"

IsometricSpritesGenerator::IsometricSpritesGenerator()
: renderer(&room3D),
  defaultShader(
          "default shader",
          "shaders/3d/default.vert", "shaders/3d/default.frag"
  ),
  riggedShader(
          "rigged shader",
          "shaders/3d/rigged.vert", "shaders/3d/default.frag"
  )
{
    room3D.initializeWithoutLevel();

    entitiesToRender = room3D.getTemplateNames();
    totalNumEntities = entitiesToRender.size();
}

float IsometricSpritesGenerator::generate()
{
    if (entitiesToRender.empty())
    {
        return 1.0;
    }

    auto toLoad = entitiesToRender.back();
    entitiesToRender.pop_back();

    entt::entity entityToRender = room3D.getTemplate(toLoad).create();
    room3D.update(0.0);

    if (auto renderToSpriteSheet = room3D.entities.try_get<RenderModelToSpriteSheet>(entityToRender))
    {
        currentlyLoading = toLoad;
        auto modelSprite = std::make_shared<ModelSprite>(*renderToSpriteSheet);

        OrthographicCamera cam(
            0.1f,
            1000.0f,
            renderToSpriteSheet->spriteSize.x,
            renderToSpriteSheet->spriteSize.y
        );

        for (int yawStep = 0; yawStep < renderToSpriteSheet->yawSteps; yawStep++)
        {
            for (int pitchStep = 0; pitchStep < renderToSpriteSheet->pitchSteps; pitchStep++)
            {
                float yaw = 360.0f * (float(yawStep) / float(renderToSpriteSheet->yawSteps));
                float pitch = 0.0f;// mix(renderToSpriteSheet->pitchMin, renderToSpriteSheet->pitchMax, float(pitchStep) / float(renderToSpriteSheet->pitchSteps));

                cam.position = vec3(renderToSpriteSheet->cameraDistance, 0.0f, 0.0f);
                cam.position = rotate(cam.position, Game::settings.graphics.isometricAngle * mu::DEGREES_TO_RAD, mu::Z);
                cam.position = rotate(cam.position, yaw * mu::DEGREES_TO_RAD, mu::Y);

                cam.lookAt(mu::ZERO_3);
                cam.update();

                auto &frame = Game::spriteSheet->addFrame(currentlyLoading, modelSprite, "default", yaw, pitch);

                Game::spriteSheet->fbo->bind();

                glViewport(
                    frame.spriteSheetOffset.x, frame.spriteSheetOffset.y,
                    renderToSpriteSheet->spriteSize.x * Game::settings.graphics.pixelsPerMeter, renderToSpriteSheet->spriteSize.y * Game::settings.graphics.pixelsPerMeter
                );
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);

                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LESS);

                Room3DRenderer::Context renderContext {
                    cam,
                    defaultShader,
                    riggedShader,
                    glfwGetTime(),   // todo.
                    1u
                };
                renderer.renderRoom(renderContext);

                Game::spriteSheet->fbo->unbind();
            }
        }
    }
    room3D.entities.destroy(entityToRender);
    return float(totalNumEntities - entitiesToRender.size()) / float(totalNumEntities);
}
