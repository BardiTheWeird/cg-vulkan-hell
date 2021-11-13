#include "scenes.hpp"

namespace lve {

    std::vector<LveGameObject> Scenes::loadTestScene1(LveDevice& device) {
        std::vector<LveGameObject> gameObjects{};

        std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(device, "models/cube.obj");

        auto cube = LveGameObject::createGameObject();
        cube.model = lveModel;
        cube.transform.translation = {.0f, .0f, 5.5f};
        // cube.transform.scale = {.5f, .5f, .5f};

        gameObjects.push_back(std::move(cube));

        std::shared_ptr<LveModel> lveModel2 = LveModel::createModelFromFile(device, "models/colored_cube.obj");

        auto cube2 = LveGameObject::createGameObject();
        cube2.model = lveModel2;
        cube2.transform.translation = {1.f, 0.f, 2.5f};
        cube2.transform.rotation = {0.f, glm::quarter_pi<float>(), 0.f};
        cube2.transform.scale = {.5f, .5f, .5f};

        gameObjects.push_back(std::move(cube2));

        std::shared_ptr<LveModel> lveModel3 = LveModel::createModelFromFile(device, "models/smooth_vase.obj");

        auto smoothVase = LveGameObject::createGameObject();
        smoothVase.model = lveModel3;
        smoothVase.transform.translation = {-1.f, 0.f, 2.5f};
        // smoothVase.transform.scale = {.5f, .5f, .5f};

        gameObjects.push_back(std::move(smoothVase));

        std::shared_ptr<LveModel> lveModel4 = LveModel::createModelFromFile(device, "models/flat_vase.obj");

        auto flatVase = LveGameObject::createGameObject();
        flatVase.model = lveModel3;
        flatVase.transform.translation = {-.5f, 0.f, 2.5f};
        flatVase.transform.scale = {1.f, 1.f, 3.f};

        gameObjects.push_back(std::move(flatVase));

        // directional lights
        LightSource directionalLight{};
        directionalLight.turnedOn = true;
        directionalLight.kind = LightSourceKind::DirectionalLight;
        directionalLight.value1 = glm::normalize(glm::vec3{1.f, -3.f, -1.f});

        auto directionalLightGameObject = LveGameObject::createGameObject();
        directionalLightGameObject.lightSource = std::make_shared<LightSource>(std::move(directionalLight));
        
        VelocityAccelerationComponent directionalLightVelocityAcceleration{};
        directionalLightVelocityAcceleration.rotationVelocity = {.5f, 0.f, 1.f};

        gameObjects.push_back(std::move(directionalLightGameObject));

        // point lights
        std::shared_ptr<LveModel> pointLightModel = LveModel::createModelFromFile(device, "models/morning-star-light-source.obj");
        auto pointLight1 = LveGameObject::createGameObject();
        pointLight1.model = pointLightModel;
        pointLight1.lightSource = std::make_shared<LightSource>(LightSource::createPoint({0.f, 0.f, 0.f}, 1000));

        pointLight1.transform.translation = {3.f, 0.f, 3.f};
        pointLight1.transform.scale = {.3f, .3f, .3f};

        // CircularMovementComponent pointLight1CircularMovement{};
        // pointLight1.circularMovement = std::make_shared<CircularMovementComponent>(pointLight1CircularMovement);
        // pointLight1.circularMovement->center = {-1.f, -1.f, -1.f};
        // pointLight1.circularMovement->speed = 3.f;

        VelocityAccelerationComponent pointLight1VelocityAcceleration{};
        pointLight1VelocityAcceleration.rotationVelocity = {.2f, .3f, .5f};

        pointLight1.velocityAcceleration = std::make_shared<VelocityAccelerationComponent>(pointLight1VelocityAcceleration);

        gameObjects.push_back(std::move(pointLight1));

        return gameObjects;
    }
       
}
