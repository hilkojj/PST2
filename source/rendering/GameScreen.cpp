
#include <game/dibidab.h>
#include <utils/quad_renderer.h>
#include "GameScreen.h"
#include "../game/Game.h"

GameScreen::GameScreen()
{
    onSessionChange = dibidab::onSessionChange += [&] {
        delete lvlScreen;
        lvlScreen = NULL;

        Session *session = dibidab::tryGetCurrentSession();
        if (session)
            onNewLevel = session->onNewLevel += [&] (Level *lvl) {
                delete lvlScreen;
                if (lvl)
                    lvlScreen = new LevelScreen(lvl);
                else lvlScreen = NULL;
            };
    };
}

bool justResized = false;
bool loadedSprites = false;
double waitAfterLoading = 0.0f;

void GameScreen::render(double deltaTime)
{
    if (!Game::uiScreenManager->noScreens())
    {
        if (lvlScreen)
            lvlScreen->render(deltaTime);
        Game::uiScreenManager->getActiveScreen().render(deltaTime);
    }
    else
    {
        glClearColor(0.2f, 0.4f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        QuadRenderer::render(Game::spriteSheet->fbo->colorTexture);

        if (!loadedSprites)
        {
            bool justConstructedGenerator = false;
            if (!spritesGenerator)
            {
                justConstructedGenerator = true;
                spritesGenerator = new IsometricSpritesGenerator;
            }
            ImGui::SetNextWindowPos(ImVec2(gu::widthPixels * 0.5f, gu::heightPixels * 0.5f), justResized ? ImGuiCond_Always : ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(300.0f, -1.0f), ImGuiCond_Appearing);
            if (ImGui::Begin("Generating isometric sprites"))
            {
                ImGui::Text("%s", spritesGenerator->currentlyLoading.empty() ? "..." : spritesGenerator->currentlyLoading.c_str());

                float progress = justConstructedGenerator ? 0.0f : spritesGenerator->generate();

                char buf[32];
                sprintf(buf, "%.1f%%", progress * 100.0f);
                ImGui::ProgressBar(progress, ImVec2(-1.0f, 0.f), buf);

                if (progress >= 1.0f)
                {
                    waitAfterLoading -= deltaTime;

                    if (waitAfterLoading <= 0.0)
                    {
                        loadedSprites = true;
                        delete spritesGenerator;
                        spritesGenerator = nullptr;
                        //Game::uiScreenManager->openScreen(asset<luau::Script>("scripts/ui_screens/StartupScreen"));
                    }
                }
            }
            ImGui::End();
        }

        ImGui::ShowDemoWindow();
    }

    ImGui::BeginMainMenuBar();

    if (ImGui::BeginMenu("Game"))
    {
        if (ImGui::BeginMenu("Graphics"))
        {
            std::string spriteUsage =
                    "Mega sprite sheet usage: " + std::to_string(int(Game::spriteSheet->getUsage() * 100.0f)) + "%";
            ImGui::MenuItem(spriteUsage.c_str(), NULL, false, false);
            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();

    justResized = false;
    if (!dibidab::settings.showDeveloperOptions)
        return;
}

void GameScreen::onResize()
{
    justResized = true;
    if (lvlScreen)
        lvlScreen->onResize();
}
