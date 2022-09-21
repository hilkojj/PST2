
_G.hudScreen = currentEngine
_G.titleScreen = false

onEvent("BeforeDelete", function()
    --loadOrCreateLevel(nil)
    if _G.hudScreen == currentEngine then
        _G.hudScreen = nil
    end
end)

_G.goToMainMenu = function()
    closeActiveScreen()
    openScreen("scripts/ui_screens/StartupScreen")
end

