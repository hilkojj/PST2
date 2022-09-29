
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
    const uvec2 &spriteOffset = orientation.animations.at("Default").at(0).spriteSheetOffset;
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

bool UIScreenWidgets::beginSecondaryMenuBar()
{
    ImGuiContext& g = *GImGui;
    g.NextWindowData.MenuBarOffsetMinVal = ImVec2(g.Style.DisplaySafeAreaPadding.x, ImMax(g.Style.DisplaySafeAreaPadding.y - g.Style.FramePadding.y, 0.0f));
    ImGui::SetNextWindowPos(ImVec2(0.0f, g.NextWindowData.MenuBarOffsetMinVal.y + g.FontBaseSize + g.Style.FramePadding.y + 4.0f));
    ImGui::SetNextWindowSize(ImVec2(g.IO.DisplaySize.x, float(Game::settings.graphics.pixelsPerMeter) + 17.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(g.Style.FramePadding.x, 3.0f));
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(177, 230, 214, 255));
    bool is_open = ImGui::Begin("##SecondaryMenuBar", NULL, window_flags);
    ImGui::PopStyleVar(3);
    if (!is_open)
    {
        ImGui::End();
        return false;
    }
    return true;
}

void UIScreenWidgets::endSecondaryMenuBar()
{
    ImGui::End();
    ImGui::PopStyleColor();
}
