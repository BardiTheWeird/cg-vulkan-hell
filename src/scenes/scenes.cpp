#include "scenes.hpp"
#include "../components/oscillator_component.hpp"

namespace lve {

    LveGameObject::Map Scenes::loadTestScene1(LveDevice& device, TextureManager& textureManager, MaterialManager& materialManager, ModelManager& modelManager) {
        LveGameObject::Map gameObjects{};

        loadCoordinateSystem(device, gameObjects, textureManager);

        textureManager.addTexture("sui-chan-guitar.jpeg", "sui-chan-guitar");
        textureManager.addTexture("ina-smile.jpg", "ina-smile");
        textureManager.addTexture("ina-super-artsy.jpg", "ina-super-artsy");
        textureManager.addTexture("suisei-fuck-this-shit.jpg", "suisei-fuck-this-shit");
        textureManager.addTexture("weird-shit.jpg", "weird-shit");

        modelManager.addModel("cube-inverted-normals", "cube-inverted-normals.obj");
        modelManager.addModel("cube", "cube.obj");
        modelManager.addModel("cube-colored", "colored_cube.obj");
        modelManager.addModel("sphere", "sphere.obj");
        modelManager.addModel("vase-smooth", "smooth_vase.obj");
        modelManager.addModel("vase-flat", "flat_vase.obj");
        modelManager.addModel("morning-star", "morning-star-light-source.obj");
        modelManager.addModel("cone", "cone.obj");

        // pointLight1.model = LveModel::createModelFromFile(device, "models/morning-star-light-source.obj");

        auto fourWallsObject = LveGameObject::createGameObject();
        fourWallsObject.modelKey = "cube-inverted-normals";
        fourWallsObject.transform.scale = {5.f, 5.f, 15.f};
        fourWallsObject.transform.translation = {0.f, -4.f, 13.f};
        
        fourWallsObject.textureKey = "suisei-fuck-this-shit";
        
        gameObjects.emplace(fourWallsObject.getId(), std::move(fourWallsObject));

        auto cube = LveGameObject::createGameObject();
        cube.modelKey = "sphere";
        cube.transform.translation = {.0f, -0.5f, 5.5f};
        cube.transform.scale = {.01f, .01f, .01f};

        cube.repeatMovement = {{8}};

        // cube.textureKey = {"weird-shit"};
        cube.textureKey = "ina-super-artsy";
        cube.material = {{ materialManager.allocateMaterial() }};

        gameObjects.emplace(cube.getId(), std::move(cube));

        auto cube2 = LveGameObject::createGameObject();
        cube2.modelKey = "cube-colored";
        cube2.transform.translation = {1.f, 0.f, 2.5f};
        cube2.transform.rotation = {0.f, glm::quarter_pi<float>(), 0.f};
        cube2.transform.scale = {.5f, .5f, .5f};

        cube2.repeatMovement = {{8}};

        cube2.textureKey = "ina-super-artsy";

        gameObjects.emplace(cube2.getId(), std::move(cube2));


        auto smoothVase = LveGameObject::createGameObject();
        smoothVase.modelKey = "vase-smooth";
        smoothVase.transform.translation = {-1.f, 0.f, 2.5f};
        // smoothVase.transform.scale = {.5f, .5f, .5f};

        smoothVase.textureKey = "sui-chan-guitar";
        smoothVase.material = {{ materialManager.allocateMaterial() }};

        gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

        auto flatVase = LveGameObject::createGameObject();
        flatVase.modelKey = "vase-smooth";
        flatVase.transform.translation = {-.5f, 0.f, 2.5f};
        flatVase.transform.scale = {1.f, 1.f, 3.f};

        flatVase.textureKey = "sui-chan-guitar";
        flatVase.material = {{ materialManager.allocateMaterial() }};
        flatVase.repeatMovement = {{ 10 }};

        gameObjects.emplace(flatVase.getId(), std::move(flatVase));

        // directional lights
        // auto directionalLightGameObject = LightSource::createDirectional(glm::normalize(glm::vec3{1.f, -3.f, -1.f}));
        // VelocityAccelerationComponent directionalLightVelocityAcceleration{};
        // directionalLightVelocityAcceleration.rotationVelocity = {.5f, 0.f, 1.f};
        // directionalLightGameObject.velocityAcceleration = {directionalLightVelocityAcceleration};

        // gameObjects.emplace(directionalLightGameObject.getId(), std::move(directionalLightGameObject));

        auto emissivityOscillator1 = OscillatorComponent::Builder()
            .SetFrequency(1.f / 16.f)
            .SetSamplingFunctionSin(1.f, .5f)
            .AddAction([](float sampledValue, LveGameObject& gameObject) {
                gameObject.material.value().emissivityMesh.w = sampledValue / 8.f;
                gameObject.material.value().updatedThisFrame = true;
                gameObject.lightSource.value().radius = 2.f + sampledValue * 10.f;
            })
            .Build();

        auto emissivityOscillator2 = OscillatorComponent::Builder()
            .SetPhase(90)
            .SetFrequency(2.f)
            .ShiftSamplingFunction(1.f)
            .ScaleSamplingFunction(.5f)
            .AddAction([](float sampledValue, LveGameObject& gameObject) {
                gameObject.material.value().emissivityMesh.w = sampledValue / 8.f;
                gameObject.material.value().updatedThisFrame = true;
                gameObject.lightSource.value().radius = 1.f + sampledValue * 3.f;
            })
            .Build();

        // point lights
        auto pointLight1 = LightSource::createPoint({0.f, 0.f, 0.f}, 5.f, {0.f, 1.f, 1.f, 1.f});
        pointLight1.transform.translation = {0.f, -3.f, 3.f};
        pointLight1.transform.scale = {.1f, .1f, .1f};
        pointLight1.modelKey = "morning-star";

        pointLight1.textureKey = "sui-chan-guitar";

        {
            MaterialComponent material{materialManager.allocateMaterial()};
            material.emissivityMesh = {0.f, 1.f, 1.f, .1f};
            pointLight1.material = {material};
        }

        pointLight1.oscillators.push_back(emissivityOscillator1);

        CircularMovementComponent pointLight1CircularMovement{};
        pointLight1CircularMovement.center = {0.f, 0.f, 3.f};
        pointLight1CircularMovement.speed = 1.f;
        pointLight1CircularMovement.rotation = glm::normalize(glm::vec3{0.f, 0.f, 1.f});
        pointLight1.circularMovement = {pointLight1CircularMovement};        

        VelocityAccelerationComponent pointLight1VelocityAcceleration{};
        pointLight1VelocityAcceleration.rotationVelocity = {.2f, .3f, .5f};

        pointLight1.velocityAcceleration = {pointLight1VelocityAcceleration};

        gameObjects.emplace(pointLight1.getId(), std::move(pointLight1));


        auto pointLight2 = LightSource::createPoint({0.f, 0.f, 0.f}, 10.f, {1.f, 1.f, 0.f, 1.f});
        pointLight2.transform.translation = {0.f, -4.f, 4.f};
        pointLight2.transform.scale = {.3f, .3f, .3f};
        pointLight2.modelKey = "morning-star";

        pointLight2.textureKey = "sui-chan-guitar";

        {
            MaterialComponent material{materialManager.allocateMaterial()};
            material.emissivityMesh = {1.f, 1.f, 0.f, .02f};
            pointLight2.material = {material};
        }

        pointLight2.oscillators.push_back(emissivityOscillator1);

        CircularMovementComponent pointLight2CircularMovement{};
        pointLight2CircularMovement.center = {0.f, 3.f, 3.f};
        pointLight2CircularMovement.speed = .8f;
        pointLight2CircularMovement.rotation = glm::normalize(glm::vec3{0.f, 1.f, 0.f});
        pointLight2.circularMovement = {pointLight2CircularMovement};        

        VelocityAccelerationComponent pointLight2VelocityAcceleration{};
        pointLight2VelocityAcceleration.rotationVelocity = {.2f, .1f, .3f};

        pointLight2.velocityAcceleration = {pointLight2VelocityAcceleration};

        gameObjects.emplace(pointLight2.getId(), std::move(pointLight2));

        return gameObjects;
    }


    void Scenes::loadCoordinateSystem(LveDevice& device, LveGameObject::Map& gameObjects, TextureManager& textureManager) {
        auto origin = LveGameObject::createGameObject();
        std::cout << "origin id: " << origin.getId() << std::endl;
        origin.modelKey = "sphere";
        origin.transform.scale = {.05f, .05f, .05f};
        origin.textureKey = "sui-chan-guitar";

        {
            VelocityAccelerationComponent va{};
            va.velocity = {0.f, 0.f, 0.1f};
            origin.velocityAcceleration = va;
        }

        auto xArrow = LveGameObject::createGameObject();
        xArrow.modelKey = "cone";
        xArrow.transform.scale = {.05f, .05f, .05f};
        xArrow.transform.translation = {0.1f, 0.f, 0.f};
        xArrow.transform.rotation = {0.f, 0.f, -glm::half_pi<float>()};
        xArrow.textureKey = "sui-chan-guitar";

        auto yArrow = LveGameObject::createGameObject();
        yArrow.modelKey = "cone";
        yArrow.transform.scale = {.05f, .05f, .05f};
        yArrow.transform.translation = {0.f, 0.1f, 0.f};
        yArrow.transform.rotation = {0.f, glm::half_pi<float>(), 0.f};
        yArrow.textureKey = "sui-chan-guitar";

        auto zArrow = LveGameObject::createGameObject();
        zArrow.modelKey = "sphere";
        zArrow.transform.scale = {.025f, .025f, .025f};
        zArrow.transform.translation = {.25f, 0.f, 0.0f};
        zArrow.transform.rotation = {glm::half_pi<float>(), 0.f, 0.f};
        zArrow.textureKey = "sui-chan-guitar";

        zArrow.oscillators.push_back(
            OscillatorComponent::GetEllipticMovement(.75f, .25f, 5.f, {0.f, 0.f, glm::quarter_pi<float>()})
            .MakeRelativeTo(0)
            .Build()
        );

        gameObjects.emplace(origin.getId(), std::move(origin));
        gameObjects.emplace(xArrow.getId(), std::move(xArrow));
        gameObjects.emplace(yArrow.getId(), std::move(yArrow));
        gameObjects.emplace(zArrow.getId(), std::move(zArrow));
    }

    LveGameObject::Map Scenes::loadSceneLab3(LveDevice& device, TextureManager& textureManager, MaterialManager& materialManager, ModelManager& modelManager) {
        LveGameObject::Map gameObjects{};

        textureManager.addTexture("monogatari-bw.jpg",          "monogatari-bw");
        textureManager.addTexture("sui-chan-guitar.jpeg",       "sui-chan-guitar");
        textureManager.addTexture("ina-smile.jpg",              "ina-smile");
        textureManager.addTexture("ina-super-artsy.jpg",        "ina-super-artsy");
        textureManager.addTexture("suisei-fuck-this-shit.jpg",  "suisei-fuck-this-shit");
        textureManager.addTexture("weird-shit.jpg",             "weird-shit");

        modelManager.addModel("cube-inverted-normals",     "cube-inverted-normals.obj");
        modelManager.addModel("cube",                      "cube.obj");
        modelManager.addModel("cube-colored",              "colored_cube.obj");
        modelManager.addModel("sphere",                    "sphere.obj");
        modelManager.addModel("vase-smooth",               "smooth_vase.obj");
        modelManager.addModel("vase-flat",                 "flat_vase.obj");
        modelManager.addModel("morning-star",              "morning-star-light-source.obj");
        modelManager.addModel("cone",                      "cone.obj");

        // skybox
        {
            auto obj = LveGameObject::createGameObject();
            obj.transform.scale = glm::vec3{50.f};
            obj.textureKey = "monogatari-bw";
            obj.modelKey = "cube-inverted-normals";
            
            MaterialComponent mat{materialManager.allocateMaterial()};
            mat.baseReflectance = 0.f;
            mat.roughness = 1.f;
            mat.emissivityMesh = {1.f, 1.f, 1.f, .001f};
            // mat.ignoreLighting = true;
            obj.material = mat;

            gameObjects.emplace(obj.getId(), std::move(obj));
        }

        // light in the middle
        {
            auto obj = LightSource::createPoint(
                glm::vec3{0.f},
                30.f
            );

            obj.transform.scale = glm::vec3{2.f};
            obj.textureKey = "monogatari-bw";
            obj.modelKey = "morning-star";
            
            MaterialComponent mat{materialManager.allocateMaterial()};
            mat.ignoreLighting = true;
            obj.material = mat;

            obj.oscillators.push_back(
                OscillatorComponent::GetLinearMovement({-20.f, 0.f, -20.f}, {20.f, 0.f, 20.f}, 5.f)
                .Build()
            );

            gameObjects.emplace(obj.getId(), std::move(obj));
        }

        return gameObjects;
    }
}
