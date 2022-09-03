
#include <imgui.h>
#include <utils/code_editor/CodeEditor.h>
#include <utils/quad_renderer.h>
#include <ecs/systems/AudioSystem.h>
#include <ecs/systems/LuaScriptsSystem.h>
#include <generated/Children.hpp>
#include "UIScreen.h"
#include "../../game/Game.h"
#include "../../ecs/systems/graphics/SpriteSystem.h"
#include "../level/room/RoomScreen.h"

UIScreen::UIScreen(const asset<luau::Script> &s)
    :
    script(s)
{
    templateFolder = "scripts/entities/ui/";

    addSystem(new AudioSystem("audio"));
    addSystem(new LuaScriptsSystem("lua functions"));

    initialize();
    UIScreen::onResize();

}

void UIScreen::render(double deltaTime)
{
    renderingOrUpdating = true;

    if (!initialized)
    {
        try
        {
            sol::protected_function_result result = luau::getLuaState().safe_script(script->getByteCode().as_string_view(), luaEnvironment);
            if (!result.valid())
                throw gu_err(result.get<sol::error>().what());
        }
        catch (std::exception &e)
        {
            std::cerr << "Error while running UIScreen script " << script.getLoadedAsset().fullPath << ":" << std::endl;
            std::cerr << e.what() << std::endl;
        }

        initialized = true;
    }

    gu::profiler::Zone z("UI");

    update(deltaTime); // todo: move this? Update ALL UIScreens? or only the active one?

    renderDebugStuff();

    renderingOrUpdating = false;
}

void UIScreen::onResize()
{
}

void UIScreen::renderDebugStuff()
{
    if (!dibidab::settings.showDeveloperOptions)
        return;

    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("UI"))
    {
        ImGui::Separator();

        ImGui::TextDisabled("Active UIScreen:");
        ImGui::Text("%s", script.getLoadedAsset().fullPath.c_str());

        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
}

UIScreen::~UIScreen()
{
}

