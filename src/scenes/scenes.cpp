#include "scenes.hpp"
#include "../components/oscillator_component.hpp"

namespace lve {

        LveGameObject::Map Scenes::loadTestScene1(LveDevice& device, TextureManager& textureManager, MaterialManager& materialManager) {
        LveGameObject::Map gameObjects{};

        loadCoordinateSystem(device, gameObjects, textureManager);

        textureManager.addTexture("sui-chan-guitar.jpeg", "sui-chan-guitar");
        textureManager.addTexture("ina-smile.jpg", "ina-smile");
        textureManager.addTexture("ina-super-artsy.jpg", "ina-super-artsy");
        textureManager.addTexture("suisei-fuck-this-shit.jpg", "suisei-fuck-this-shit");
        textureManager.addTexture("masterball.png", "masterball");
        textureManager.addTexture("weird-shit.jpg", "weird-shit");

        std::optional<std::string> sui_chan_guitar = {"sui-chan-guitar"};
        std::optional<std::string> ina_smile = {"ina-smile"};
        std::optional<std::string> ina_super_artsy = {"ina-super-artsy"};
        std::optional<std::string> suisei_fuck_this_shit = {"suisei-fuck-this-shit"};
        std::optional<std::string> masterball = {"masterball"};

        std::shared_ptr<LveModel> whiteCubeInvertedNormals = LveModel::createModelFromFile(device, "models/cube-inverted-normals.obj");
        std::shared_ptr<LveModel> whiteCubeModel = LveModel::createModelFromFile(device, "models/cube.obj");
        std::shared_ptr<LveModel> coloreCubeModel = LveModel::createModelFromFile(device, "models/colored_cube.obj");

        std::shared_ptr<LveModel> originModel = LveModel::createModelFromFile(device, "models/sphere.obj");
        // std::shared_ptr<LveModel> masterballModel = LveModel::createModelFromFile(device, "models/Masterball.obj");

        auto fourWallsObject = LveGameObject::createGameObject();
        fourWallsObject.model = whiteCubeInvertedNormals;
        fourWallsObject.transform.scale = {5.f, 5.f, 15.f};
        fourWallsObject.transform.translation = {0.f, -4.f, 13.f};
        
        fourWallsObject.textureKey = suisei_fuck_this_shit;
        
        gameObjects.emplace(fourWallsObject.getId(), std::move(fourWallsObject));

        auto cube = LveGameObject::createGameObject();
        cube.model = originModel;
        cube.transform.translation = {.0f, -0.5f, 5.5f};
        cube.transform.scale = {.01f, .01f, .01f};

        cube.repeatMovement = {{8}};

        // cube.textureKey = {"weird-shit"};
        cube.textureKey = ina_super_artsy;
        cube.material = {{ materialManager.allocateMaterial() }};

        gameObjects.emplace(cube.getId(), std::move(cube));

        auto cube2 = LveGameObject::createGameObject();
        cube2.model = coloreCubeModel;
        cube2.transform.translation = {1.f, 0.f, 2.5f};
        cube2.transform.rotation = {0.f, glm::quarter_pi<float>(), 0.f};
        cube2.transform.scale = {.5f, .5f, .5f};

        cube2.repeatMovement = {{8}};

        cube2.textureKey = ina_super_artsy;

        gameObjects.emplace(cube2.getId(), std::move(cube2));

        std::shared_ptr<LveModel> lveModel3 = LveModel::createModelFromFile(device, "models/smooth_vase.obj");

        auto smoothVase = LveGameObject::createGameObject();
        smoothVase.model = lveModel3;
        smoothVase.transform.translation = {-1.f, 0.f, 2.5f};
        // smoothVase.transform.scale = {.5f, .5f, .5f};

        smoothVase.textureKey = sui_chan_guitar;
        smoothVase.material = {{ materialManager.allocateMaterial() }};

        auto ellipsisOscillator = std::make_shared<OscillatorComponent>();
        ellipsisOscillator->frequency = 1.f / 4.f;
        ellipsisOscillator->actOnGameObject = [](float sampledValue, float frameTime, LveGameObject& gameObject, std::vector<MoveEvent>& moveEvents) {
            float t = (sampledValue + 1.f) / 2.f;
            gameObject.transform.translation.x = t * 2;
            gameObject.transform.translation.z = t * 4;
        };

        smoothVase.oscillators.push_back(ellipsisOscillator);

        gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

        std::shared_ptr<LveModel> lveModel4 = LveModel::createModelFromFile(device, "models/flat_vase.obj");

        auto flatVase = LveGameObject::createGameObject();
        flatVase.model = lveModel3;
        flatVase.transform.translation = {-.5f, 0.f, 2.5f};
        flatVase.transform.scale = {1.f, 1.f, 3.f};

        flatVase.textureKey = sui_chan_guitar;
        flatVase.material = {{ materialManager.allocateMaterial() }};
        flatVase.repeatMovement = {{ 10 }};

        gameObjects.emplace(flatVase.getId(), std::move(flatVase));

        // directional lights
        // auto directionalLightGameObject = LightSource::createDirectional(glm::normalize(glm::vec3{1.f, -3.f, -1.f}));
        // VelocityAccelerationComponent directionalLightVelocityAcceleration{};
        // directionalLightVelocityAcceleration.rotationVelocity = {.5f, 0.f, 1.f};
        // directionalLightGameObject.velocityAcceleration = {directionalLightVelocityAcceleration};

        // gameObjects.emplace(directionalLightGameObject.getId(), std::move(directionalLightGameObject));

        // auto emissivityOscillator1 = OscillatorComponent::Builder()
        //     .SetFrequency(0.25f)
        //     .AddAction([](float sampledValue, LveGameObject& gameObject) {
        //         gameObject.material.value().emissivityMesh.w = (sampledValue + 1.f) / 2.f;
        //     })
        //     .Build();

        auto emissivityOscillator1 = std::make_shared<OscillatorComponent>();
        emissivityOscillator1->frequency = 1.f / 16.f;
        emissivityOscillator1->actOnGameObject = [](float sampledValue, float frameTime, LveGameObject& gameObject, std::vector<MoveEvent>& moveEvents) {
            gameObject.material.value().emissivityMesh.w = (sampledValue + 1.f) / 2.f / 8.f;
            gameObject.material.value().updatedThisFrame = true;
            gameObject.lightSource.value().radius = 2.f + (sampledValue + 1.f) / 2.f * 10.f;
            // gameObject.lightSource.value().radius = 0.f;
        };
        // emissivityOscillator1->samplingFunction = [](float t){return t;};

        auto emissivityOscillator2 = std::make_shared<OscillatorComponent>();
        emissivityOscillator2->curValue = .5f;
        emissivityOscillator2->frequency = 2.f;
        emissivityOscillator2->actOnGameObject = [](float sampledValue, float frameTime, LveGameObject& gameObject, std::vector<MoveEvent>& moveEvents) {
            gameObject.material.value().emissivityMesh.w = (sampledValue + 1.f) / 2.f / 8.f;
            gameObject.material.value().updatedThisFrame = true;
            gameObject.lightSource.value().radius = 1.f + (sampledValue + 1.f) / 2.f * 3.f;
            // gameObject.lightSource.value().radius = 0.f;
        };

        // point lights
        auto pointLight1 = LightSource::createPoint({0.f, 0.f, 0.f}, 5.f, {0.f, 1.f, 1.f, 1.f});
        pointLight1.transform.translation = {0.f, -3.f, 3.f};
        pointLight1.transform.scale = {.1f, .1f, .1f};
        pointLight1.model = LveModel::createModelFromFile(device, "models/morning-star-light-source.obj");

        pointLight1.textureKey = sui_chan_guitar;

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
        pointLight2.model = LveModel::createModelFromFile(device, "models/morning-star-light-source.obj");

        pointLight2.textureKey = sui_chan_guitar;

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

        // for (auto& kv: gameObjects) {
        //     auto& obj = kv.second;
        //     if (obj.material.has_value()) {
        //         materialManager.updateMaterial(obj.material.value());
        //     }
        // }

        return gameObjects;
    }


    void Scenes::loadCoordinateSystem(LveDevice& device, LveGameObject::Map& gameObjects, TextureManager& textureManager) {
        std::shared_ptr<LveModel> originModel = LveModel::createModelFromFile(device, "models/sphere.obj");
        std::shared_ptr<LveModel> arrowModel = LveModel::createModelFromFile(device, "models/cone.obj");

        std::optional<std::string> sui_chan_guitar = {"sui-chan-guitar"};

        auto origin = LveGameObject::createGameObject();
        origin.model = originModel;
        origin.transform.scale = {.05f, .05f, .05f};
        origin.textureKey = sui_chan_guitar;

        auto xArrow = LveGameObject::createGameObject();
        xArrow.model = arrowModel;
        xArrow.transform.scale = {.05f, .05f, .05f};
        xArrow.transform.translation = {0.1f, 0.f, 0.f};
        xArrow.transform.rotation = {0.f, 0.f, -glm::half_pi<float>()};
        xArrow.textureKey = sui_chan_guitar;

        auto yArrow = LveGameObject::createGameObject();
        yArrow.model = arrowModel;
        yArrow.transform.scale = {.05f, .05f, .05f};
        yArrow.transform.translation = {0.f, 0.1f, 0.f};
        yArrow.transform.rotation = {0.f, glm::half_pi<float>(), 0.f};
        yArrow.textureKey = sui_chan_guitar;

        auto zArrow = LveGameObject::createGameObject();
        zArrow.model = arrowModel;
        zArrow.transform.scale = {.05f, .05f, .05f};
        zArrow.transform.translation = {0.f, 0.f, 0.1f};
        zArrow.transform.rotation = {glm::half_pi<float>(), 0.f, 0.f};
        zArrow.textureKey = sui_chan_guitar;

        gameObjects.emplace(origin.getId(), std::move(origin));
        gameObjects.emplace(xArrow.getId(), std::move(xArrow));
        gameObjects.emplace(yArrow.getId(), std::move(yArrow));
        gameObjects.emplace(zArrow.getId(), std::move(zArrow));
    }
}
