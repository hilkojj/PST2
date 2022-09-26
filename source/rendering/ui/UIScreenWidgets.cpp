
#include "UIScreenWidgets.h"

#include "../../game/Game.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace
{
    bool spriteImageButton(const char *buttonID, const uvec2 &spriteOffset, const ImVec2 &imgSize, bool inset, bool flipX)
    {
        auto imgPtr = (void*)(intptr_t) Game::spriteSheet->fbo->colorTexture->id;

        ImVec2 uv0(float(spriteOffset.x + (flipX ? imgSize.x : 0)) / float(Game::spriteSheet->size), float(spriteOffset.y + imgSize.y) / float(Game::spriteSheet->size));
        ImVec2 uv1(float(spriteOffset.x + (flipX ? 0 : imgSize.x)) / float(Game::spriteSheet->size), float(spriteOffset.y) / float(Game::spriteSheet->size));

        ImVec4 bgCol = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);         // no background
        ImVec4 tintCol = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);       // No tint

        ImGui::PushID(buttonID);
        bool result = ImGui::ImageButton(imgPtr, imgSize, uv0, uv1, 5, bgCol, tintCol, inset ? ImGuiButtonFlags_Win98Inset : 0);
        ImGui::PopID();
        return result;
    }
}

bool UIScreenWidgets::modelSpriteImageButton(const char *buttonID, const char *modelName, bool inset, bool flipX, float scale)
{
    const SharedModelSprite &modelSprite = Game::spriteSheet->getModelSpriteByName(modelName);
    return modelSpriteImageButton(buttonID, modelSprite, modelSprite->getClosestOrientation(0, 0), inset, flipX, scale);
}

bool UIScreenWidgets::modelSpriteImageButton(const char *buttonID, const SharedModelSprite &modelSprite, const ModelSprite::Orientation &orientation, bool inset, bool flipX, float scale)
{
    const uvec2 &spriteOffset = orientation.animations.at("default").at(0).spriteSheetOffset;
    const ImVec2 imgSize(
        modelSprite->renderProperties.spriteSize.x * float(Game::settings.graphics.pixelsPerMeter) * scale,
        modelSprite->renderProperties.spriteSize.y * float(Game::settings.graphics.pixelsPerMeter) * scale
    );

    return spriteImageButton(buttonID, spriteOffset, imgSize, inset, flipX);
}

bool UIScreenWidgets::asepriteImageButton(const char *buttonID, const asset<aseprite::Sprite> &sprite, uint frame, bool inset, bool flipX, float scale)
{
    const MegaSpriteSheet::SubSheet &subSheet = Game::spriteSheet->spriteInfo(sprite.get());

    const uvec2 &spriteOffset = subSheet.frameOffsets.at(frame);
    const ImVec2 imgSize(float(sprite->width) * scale, float(sprite->height) * scale);

    return spriteImageButton(buttonID, spriteOffset, imgSize, inset, flipX);
}
