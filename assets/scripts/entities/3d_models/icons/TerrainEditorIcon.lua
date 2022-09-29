
function create(model)

    loadModels("assets/models/tile_shapes.glb", false)

    setComponents(model, {
        Transform {
            scale = vec3(1)
        },
        RenderModel {
            modelName = "TerrainEditorIcon"
        },
        RenderModelToSpriteSheet {
            yawSteps = 1,
            pitchSteps = 1,
            spriteSize = vec2(1)   -- meters.
        }
    })

    local light = createChild(model, "light")
    setComponents(light, {
        Transform {
            position = vec3(1.6, 3, 0)
        },
        PointLight {
            color = vec3(0.8, 0.8, 1.6)
        }
    })

    local sun = createChild(model, "sun")
    local sunRot = quat:new()
    sunRot.x = -100
    sunRot.y = 70
    setComponents(sun, {
        Transform {
            rotation = sunRot
        },
        PointLight {
            color = vec3(0.3, 0.2, 0.2)
        }
    })
    finishCurrentAnimation()
end
