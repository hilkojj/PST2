
animatePos = function(banana)

    local goal = vec3(
            currentRoom.tileMapSize.x * math.random(),
            1.0,
            currentRoom.tileMapSize.z * math.random()
    )

    local duration = (1.0 + math.random()) * 17

    component.Transform.animate(banana, "position", goal, duration, "pow2", animatePos)
end


animateRot = function(banana)

    local goal = quat:new()

    goal.y = math.random() * 360

    local duration = (1.0 + math.random()) * 0.2

    component.Transform.animate(banana, "rotation", goal, duration, "pow2", animateRot)
end


function create(banana)

    setComponents(banana, {
        Transform {
            position = vec3(1, 1, 1)
        },
        ModelSpriteView {
            modelName = "Banana"
        }
    })

    setTimeout(banana, .1, animatePos)
    setTimeout(banana, .1, animateRot)

end
