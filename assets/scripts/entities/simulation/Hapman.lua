

function create(hapman)

    setComponents(hapman, {
        Transform {
            position = vec3(1, 1, 1)
        },
        ModelSpriteView {
            modelName = "Hapman",
            animation = "Default",
            loop = true
        }
    })

    setUpdateFunction(hapman, 10, function()
        local view = component.ModelSpriteView.getFor(hapman)

        view.animation = "Bite"
        view.frame = 0
        view.frameTimer = 0
        view.loop = false
        view.speedMultiplier = 3
    end)

    onEntityEvent(hapman, "ModelSpriteAnimationFinished", function()
        local view = component.ModelSpriteView.getFor(hapman)

        view.animation = "Default"
        view.frame = 0
        view.frameTimer = 0
        view.loop = true
    end)
end
