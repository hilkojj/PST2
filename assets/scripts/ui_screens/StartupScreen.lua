
_G.titleScreen = true

_G.joinedSession = false

startupArgs = getGameStartupArgs()
if not _G.joinedSession then

    saveGamePath = startupArgs["--single-player"] or "saves/default_save.dibdab"
    startSinglePlayerSession(saveGamePath)

    username = startupArgs["--username"] or "poopoo"
    joinSession(username, function(declineReason)

        tryCloseGame()
        error("couldn't join session: "..declineReason)
    end)
    _G.joinedSession = true
end

onEvent("BeforeDelete", function()
    print("startup screen done..")
end)

function startLevel()
    closeActiveScreen()
    openScreen("scripts/ui_screens/LevelScreen")
end

startLevel()
