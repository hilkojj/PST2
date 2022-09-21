
#include <imgui.h>
#include <utils/code_editor/CodeEditor.h>
#include <utils/quad_renderer.h>
#include <ecs/systems/AudioSystem.h>
#include <ecs/systems/LuaScriptsSystem.h>
#include <game/dibidab.h>
#include <game/session/SingleplayerSession.h>
#include "UIScreen.h"
#include "../../game/Game.h"
#include "../../level/isometric_room/IsoRoom.h"
#include "../../ecs/systems/graphics/SpriteSystem.h"

#define LVL_EXTENSION ".pst2"

namespace UIScreenWidgets
{
    bool justResized = false;
    bool showAbout = false;

    namespace
    {
        void loadLevel(Level *lvl)
        {
            auto &session = dibidab::getCurrentSession();
            auto singleplayerSession = dynamic_cast<SingleplayerSession *>(&session);
            if (singleplayerSession)
            {
                singleplayerSession->setLevel(lvl);
                Game::uiScreenManager->closeActiveScreen();
                // TODO: either completely remove scripts for UI screens, or add ImGUI bindings to lua.
                if (lvl)
                {
                    Game::uiScreenManager->openScreen(asset<luau::Script>("scripts/ui_screens/LevelScreen"));
                }
                else
                {
                    Game::uiScreenManager->openScreen(asset<luau::Script>("scripts/ui_screens/StartupScreen"));
                }
            }
            else delete lvl;
        }
    }

    void showMainMenuWindow()
    {
        static bool showNewScenario = false;

        ImGui::SetNextWindowPos(ImVec2(gu::widthPixels * 0.5f, gu::heightPixels * 0.5f), justResized ? ImGuiCond_Always : ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(400.0f, -1.0f), ImGuiCond_Appearing);
        if (ImGui::Begin("Parking Simulator Tycoon II"))
        {
            if (ImGui::Button("New scenario"))
            {
                showNewScenario = true;
            }
            ImGui::SameLine(0.0f, 16.0f);

            static std::vector<std::string> lvlPaths;

            if (ImGui::Button("Load scenario"))
            {
                ImGui::OpenPopup("load_lvl_file");
                lvlPaths.clear();
                File::iterateDirectoryRecursively("saves/", [](auto path, auto isDir) {
                    if (!isDir && stringEndsWith(path, LVL_EXTENSION))
                    {
                        lvlPaths.push_back(path);
                    }
                });
            }
            if (ImGui::BeginPopup("load_lvl_file"))
            {
                if (lvlPaths.empty())
                {
                    ImGui::TextDisabled("No saved scenarios found.");
                }

                for (auto &path : lvlPaths)
                {
                    if (ImGui::Selectable(path.c_str()))
                    {
                        loadLevel(new Level(path.c_str()));
                    }
                }
                ImGui::EndPopup();
            }

            /*
            TODO:
            ImGui::SameLine(0.0f, 16.0f);
            if (ImGui::Button("About"))
            {
                showAbout = true;
            }
             */
            ImGui::SameLine(0.0f, 16.0f);
            if (ImGui::Button("Exit game"))
            {
                gu::setShouldClose(true);
            }
        }
        ImGui::End();

        if (showNewScenario)
        {
            ImGui::SetNextWindowSize(ImVec2(250.0f, -1.0f), ImGuiCond_Appearing);
            if (ImGui::Begin("Create new scenario", &showNewScenario))
            {
                static char lvlName[64] = "";
                ImGui::InputTextWithHint("", "Level file name", lvlName, IM_ARRAYSIZE(lvlName));
                if (ImGui::Button("Create"))
                {
                    std::string lvlNameStr(lvlName);
                    std::string path = "saves/" + lvlNameStr + LVL_EXTENSION;
                    if (lvlNameStr.empty())
                    {
                        ImGui::OpenPopup("Invalid input");
                    }
                    else if (File::exists(path.c_str()))
                    {
                        ImGui::OpenPopup("Name already in use");
                    }
                    else
                    {
                        auto lvl = new Level(path.c_str());
                        lvl->addRoom(new IsoRoom(uvec3(128)));
                        lvl->spawnRoom = lvl->getRoom(0).name = "main";
                        loadLevel(lvl);
                    }
                }

                if (ImGui::BeginPopupModal("Invalid input"))
                {
                    ImGui::Text("Please enter a valid level file name.");
                    if (ImGui::Button("OK"))
                    {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
                if (ImGui::BeginPopupModal("Name already in use"))
                {
                    ImGui::Text("A level file with this name already exists.");
                    if (ImGui::Button("OK"))
                    {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
            }
            ImGui::End();
        }
    }

    void showAboutWindow()
    {
        if (ImGui::Begin("About", &showAbout))
        {
            ImGui::Text("WIP.");
        }
        ImGui::End();
    }

    void showMainMenuBar()
    {
        ImGui::BeginMainMenuBar();

        if (ImGui::BeginMenu("Game"))
        {
            if (dibidab::tryGetCurrentSession() && dibidab::getCurrentSession().getLevel())
            {
                if (ImGui::MenuItem("Save"))
                {
                    dibidab::getCurrentSession().getLevel()->save(nullptr);
                }
                if (ImGui::MenuItem("Save & exit to menu"))
                {
                    loadLevel(nullptr);
                }
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    void showSaveOnExitPopupIfNeeded()
    {
        static bool canClose = false;

        static auto canCloseDelegate = gu::canClose += [] {

            if (!dibidab::tryGetCurrentSession() || !dibidab::getCurrentSession().getLevel())
            {
                return true;
            }
            return canClose;
        };

        if (gu::shouldClose() && dibidab::tryGetCurrentSession() && dibidab::getCurrentSession().getLevel())
        {
            ImGui::OpenPopup("Save game before exit?");

            if (ImGui::BeginPopupModal("Save game before exit?", nullptr, ImGuiWindowFlags_NoSavedSettings))
            {
                ImGui::Text("Do you want to save your progress before exiting the game?");
                if (ImGui::Button("Yes"))
                {
                    dibidab::getCurrentSession().getLevel()->saveOnDestruct = true;
                    canClose = true;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("No"))
                {
                    dibidab::getCurrentSession().getLevel()->saveOnDestruct = false;
                    canClose = true;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel"))
                {
                    canClose = false;
                    gu::setShouldClose(false);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
    }
}


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


    UIScreenWidgets::showSaveOnExitPopupIfNeeded();
    UIScreenWidgets::showMainMenuBar();

    if (luaEnvironment["showMainMenu"] == true)
    {
        UIScreenWidgets::showMainMenuWindow();
    }
    if (UIScreenWidgets::showAbout)
    {
        UIScreenWidgets::showAboutWindow();
    }

    renderDebugStuff();

    renderingOrUpdating = false;
    UIScreenWidgets::justResized = false;
}

void UIScreen::onResize()
{
    UIScreenWidgets::justResized = true;
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

    if (ImGui::BeginMenu("Game"))
    {
        if (ImGui::MenuItem("Open 3D room"))
        {
            Level *level3d = new Level;
            level3d->addRoom(new Room3D);
            UIScreenWidgets::loadLevel(level3d);
            level3d->getRoom(0).getTemplate("Camera").create();
        }
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
}

UIScreen::~UIScreen()
{
}

