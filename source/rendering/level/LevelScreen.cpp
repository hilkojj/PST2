
#include "LevelScreen.h"
#include <game/dibidab.h>
#include <imgui.h>

LevelScreen::LevelScreen(Level *lvl) : lvl(lvl)
{
    onPlayerEnteredRoom = lvl->onPlayerEnteredRoom += [this](Room *room, int playerId)
    {
        auto localPlayer = dibidab::getCurrentSession().getLocalPlayer();
        if (!localPlayer || localPlayer->id != playerId)
            return;
        std::cout << "Local player entered room. Show RoomScreen\n";
        showRoom(room);
    };

    onRoomDeletion = lvl->beforeRoomDeletion += [this](Room *r)
    {
        if (room3DScreen && r == room3DScreen->room)
        {
            delete room3DScreen;
            room3DScreen = NULL;
        }
        if (isoRoomScreen && r == isoRoomScreen->room)
        {
            delete isoRoomScreen;
            isoRoomScreen = NULL;
        }
    };
}

void LevelScreen::render(double deltaTime)
{
    renderDebugTools();
    if (room3DScreen)
        room3DScreen->render(deltaTime);
    if (isoRoomScreen)
        isoRoomScreen->render(deltaTime);
}

void LevelScreen::onResize()
{
    if (room3DScreen)
        room3DScreen->onResize();
    if (isoRoomScreen)
        isoRoomScreen->onResize();
}

void LevelScreen::renderDebugTools()
{
    return;
    if (!dibidab::settings.showDeveloperOptions)
        return;

    ImGui::BeginMainMenuBar();

    if (ImGui::BeginMenu("Level"))
    {
        auto str = std::to_string(lvl->getNrOfRooms()) + " room(s) active.";
        ImGui::MenuItem(str.c_str(), NULL, false, false);

        if (ImGui::MenuItem("Create new Room"))
            lvl->addRoom(new Room3D);

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}

void LevelScreen::showRoom(Room *r)
{
    delete room3DScreen;
    room3DScreen = nullptr;
    delete isoRoomScreen;
    isoRoomScreen = nullptr;

    if (r == nullptr)
        return;

    if (auto r3d = dynamic_cast<Room3D *>(r))
    {
        room3DScreen = new Room3DScreen(r3d);
        room3DScreen->onResize();
    }
    else if (auto rIso = dynamic_cast<IsoRoom *>(r))
    {
        isoRoomScreen = new IsoRoomScreen(rIso);
        isoRoomScreen->onResize();
    }
}

