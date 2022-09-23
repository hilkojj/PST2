
defaultArgs({
    modelName = nil
})

function create(model, args)

    loadModels("assets/models/tile_shapes.glb", false)

    if args.modelName == nil then
        return
    end

    setComponents(model, {
        Transform {
            scale = vec3(1)
        },
        RenderModel {
            modelName = args.modelName
        },
        RenderModelToSpriteSheet {
            yawSteps = 8,
            pitchSteps = 1,
            spriteSize = vec2(1.9, 1.8),   -- meters.
            showGrid = true,
            gridOffset = vec3(0.5, -0.25, 0.5),
            gridSize = 3,
            gridColor = vec3(1.0, 0.7, 0.7) * vec3(0.8)
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

end

