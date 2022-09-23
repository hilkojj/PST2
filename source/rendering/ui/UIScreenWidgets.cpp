
#include "UIScreenWidgets.h"

#include "../../game/Game.h"

#include <imgui.h>
#include <imgui_internal.h>

bool UIScreenWidgets::modelSpriteImageButton(const char *buttonID, const char *modelName, bool inset, bool flipX)
{
    const SharedModelSprite &modelSprite = Game::spriteSheet->getModelSpriteByName(modelName);
    return modelSpriteImageButton(buttonID, modelSprite, modelSprite->getClosestOrientation(0, 0), inset, flipX);
}

bool UIScreenWidgets::modelSpriteImageButton(const char *buttonID, const SharedModelSprite &modelSprite, const ModelSprite::Orientation &orientation, bool inset, bool flipX)
{
    const ivec2 &spriteOffset = orientation.animations.at("default").at(0).spriteSheetOffset;

    auto imgPtr = (void*)(intptr_t) Game::spriteSheet->fbo->colorTexture->id;

    ImVec2 imgSize(modelSprite->renderProperties.spriteSize.x * Game::settings.graphics.pixelsPerMeter, modelSprite->renderProperties.spriteSize.y * Game::settings.graphics.pixelsPerMeter);
    ImVec2 uv0(float(spriteOffset.x + (flipX ? imgSize.x : 0)) / float(Game::spriteSheet->size), float(spriteOffset.y + imgSize.y) / float(Game::spriteSheet->size));
    ImVec2 uv1(float(spriteOffset.x + (flipX ? 0 : imgSize.x)) / float(Game::spriteSheet->size), float(spriteOffset.y) / float(Game::spriteSheet->size));

    ImVec4 bgCol = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);         // no background
    ImVec4 tintCol = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);       // No tint

    ImGui::PushID(buttonID);
    bool result = ImGui::ImageButton(imgPtr, imgSize, uv0, uv1, 5, bgCol, tintCol, inset ? ImGuiButtonFlags_Win98Inset : 0);
    ImGui::PopID();
    return result;
}
