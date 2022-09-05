
function create(model)

    loadModels("assets/models/banana.glb", false)

    setComponents(model, {
        Transform {
            scale = vec3(1)
        },
        RenderModel {
            modelName = "Banana"
        },
        RenderModelToSpriteSheet {
            yawSteps = 8,
            pitchSteps = 1,
            pitchMin = -45,
            pitchMax = 45,
            spriteSize = uvec2(4, 3)    -- meters.
        }
    })

    local light = createChild(model, "light")
    setComponents(light, {
        Transform(),
        DirectionalLight()
    })

end
