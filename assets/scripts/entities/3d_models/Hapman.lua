
function create(model)

    loadRiggedModels("assets/models/hapman.glb", false)

    setComponents(model, {
        Transform {
        },
        RenderModel {
            modelName = "Hapman"
        },
        RenderModelToSpriteSheet {
            yawSteps = 8,
            pitchSteps = 1,
            pitchMin = -45,
            pitchMax = 45,
            spriteSize = vec2(8, 8),    -- meters.

            animationNames = {"Default", "Bite"},
            fps = 2
        }
    })

    local light = createChild(model, "light")
    setComponents(light, {
        Transform(),
        DirectionalLight()
    })

    onEntityEvent(model, "AnimationStarted", function(animName)

        local animTable = {
            Default = function ()
                component.Rigged.getFor(model).playingAnimations = {
                    PlayAnimation {
                        name = "Idle"
                    }
                }
            end,
            Bite = function ()
                component.Rigged.getFor(model).playingAnimations = {
                    PlayAnimation {
                        name = "Bite"
                    }
                }
            end
        }
        print(animName)
        animTable[animName]()
    end)
    onEntityEvent(model, "AnimationFinished", finishCurrentAnimation)
end
