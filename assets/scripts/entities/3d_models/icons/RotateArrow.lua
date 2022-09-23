
function create(model)

    loadModels("assets/models/rotate_arrow.glb", false)

    setComponents(model, {
        Transform {
            scale = vec3(1)
        },
        RenderModel {
            modelName = "RotateArrow"
        },
        RenderModelToSpriteSheet {
            yawSteps = 1,
            baseYaw = 0,
            pitchSteps = 1,
            spriteSize = vec2(1.25, 1.25)   -- meters.
        }
    })

    local light = createChild(model, "light")
    setComponents(light, {
        Transform {
            position = vec3(0.8, 1.5, 0)
        },
        PointLight {
            color = vec3(1.1, 3.0, 2.0)
        }
    })

end
