
function create(model)

    loadModels("assets/models/banana.glb", false)

    setComponents(model, {
        Transform(),
        RenderModel {
            modelName = "Banana"
        }
    })

    local light = createChild(model, "light")
    setComponents(light, {
        Transform(),
        DirectionalLight()
    })

end
