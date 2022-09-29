
function create(model)

    loadModels("assets/models/banana.glb", false)

    setComponents(model, {
        Transform {
        },
        RenderModel {
            modelName = "Banana"
        },
        RenderModelToSpriteSheet {
            yawSteps = 24,
            pitchSteps = 1,
            pitchMin = -45,
            pitchMax = 45,
            spriteSize = vec2(4, 3)    -- meters.
        }
    })

    local light = createChild(model, "light")
    setComponents(light, {
        Transform(),
        DirectionalLight()
    })

    finishCurrentAnimation()
end
