
function create(player)
    setUpdateFunction(player, 0.05, function()

        local banana = createChild(player)
        applyTemplate(banana, "Banana")
        local trans = component.Transform.getFor(banana)
        trans.position.x = math.random() * currentRoom.tileMapSize.x
        trans.position.y = 1
        trans.position.z = math.random() * currentRoom.tileMapSize.z
        component.DespawnAfter.getFor(banana).time = 60

    end)
end
