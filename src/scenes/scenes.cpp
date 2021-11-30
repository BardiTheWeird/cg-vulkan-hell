#include "scenes.hpp"

namespace lve {

    LveGameObject::Map Scenes::loadTestScene1(LveDevice& device) {
        LveGameObject::Map gameObjects{};

        loadCoordinateSystem(device, gameObjects);

        std::shared_ptr<LveModel> whiteCubeInvertedNormals = LveModel::createModelFromFile(device, "models/cube-inverted-normals.obj");
        std::shared_ptr<LveModel> whiteCubeModel = LveModel::createModelFromFile(device, "models/cube.obj");
        std::shared_ptr<LveModel> coloreCubeModel = LveModel::createModelFromFile(device, "models/colored_cube.obj");

        auto fourWallsObject = LveGameObject::createGameObject();
        fourWallsObject.model = whiteCubeInvertedNormals;
        fourWallsObject.transform.scale = {5.f, 5.f, 15.f};
        fourWallsObject.transform.translation = {0.f, -4.f, 13.f};
        
        gameObjects.emplace(fourWallsObject.getId(), std::move(fourWallsObject));

        auto cube = LveGameObject::createGameObject();
        cube.model = whiteCubeModel;
        cube.transform.translation = {.0f, .0f, 5.5f};
        // cube.transform.scale = {.5f, .5f, .5f};

        gameObjects.emplace(cube.getId(), std::move(cube));

        auto cube2 = LveGameObject::createGameObject();
        cube2.model = coloreCubeModel;
        cube2.transform.translation = {1.f, 0.f, 2.5f};
        cube2.transform.rotation = {0.f, glm::quarter_pi<float>(), 0.f};
        cube2.transform.scale = {.5f, .5f, .5f};

        gameObjects.emplace(cube2.getId(), std::move(cube2));

        std::shared_ptr<LveModel> lveModel3 = LveModel::createModelFromFile(device, "models/smooth_vase.obj");

        auto smoothVase = LveGameObject::createGameObject();
        smoothVase.model = lveModel3;
        smoothVase.transform.translation = {-1.f, 0.f, 2.5f};
        // smoothVase.transform.scale = {.5f, .5f, .5f};

        gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

        std::shared_ptr<LveModel> lveModel4 = LveModel::createModelFromFile(device, "models/flat_vase.obj");

        auto flatVase = LveGameObject::createGameObject();
        flatVase.model = lveModel3;
        flatVase.transform.translation = {-.5f, 0.f, 2.5f};
        flatVase.transform.scale = {1.f, 1.f, 3.f};

        gameObjects.emplace(flatVase.getId(), std::move(flatVase));

        // directional lights
        auto directionalLightGameObject = LightSource::createDirectional(glm::normalize(glm::vec3{1.f, -3.f, -1.f}));
        // VelocityAccelerationComponent directionalLightVelocityAcceleration{};
        // directionalLightVelocityAcceleration.rotationVelocity = {.5f, 0.f, 1.f};
        // directionalLightGameObject.velocityAcceleration = std::make_shared<VelocityAccelerationComponent>(directionalLightVelocityAcceleration);

        // gameObjects.emplace(directionalLightGameObject.getId(), std::move(directionalLightGameObject));

        // point lights
        auto pointLight1 = LightSource::createPoint({0.f, 0.f, 0.f}, 5.f, {0.f, 1.f, 1.f, 0.f});
        pointLight1.transform.translation = {0.f, -3.f, 3.f};
        pointLight1.transform.scale = {.1f, .1f, .1f};
        pointLight1.model = LveModel::createModelFromFile(device, "models/morning-star-light-source.obj");

        CircularMovementComponent pointLight1CircularMovement{};
        pointLight1CircularMovement.center = {0.f, 0.f, 3.f};
        pointLight1CircularMovement.speed = 1.f;
        pointLight1CircularMovement.rotation = glm::normalize(glm::vec3{0.f, 0.f, 1.f});
        pointLight1.circularMovement = std::make_shared<CircularMovementComponent>(pointLight1CircularMovement);        

        VelocityAccelerationComponent pointLight1VelocityAcceleration{};
        pointLight1VelocityAcceleration.rotationVelocity = {.2f, .3f, .5f};

        pointLight1.velocityAcceleration = std::make_shared<VelocityAccelerationComponent>(pointLight1VelocityAcceleration);

        gameObjects.emplace(pointLight1.getId(), std::move(pointLight1));


        auto pointLight2 = LightSource::createPoint({0.f, 0.f, 0.f}, 5.f, {1.f, 1.f, 0.f, 0.f});
        pointLight2.transform.translation = {0.f, -4.f, 4.f};
        pointLight2.transform.scale = {.3f, .3f, .3f};
        pointLight2.model = LveModel::createModelFromFile(device, "models/morning-star-light-source.obj");

        CircularMovementComponent pointLight2CircularMovement{};
        pointLight2CircularMovement.center = {0.f, 3.f, 3.f};
        pointLight2CircularMovement.speed = .8f;
        pointLight2CircularMovement.rotation = glm::normalize(glm::vec3{0.f, 1.f, 0.f});
        pointLight2.circularMovement = std::make_shared<CircularMovementComponent>(pointLight2CircularMovement);        

        VelocityAccelerationComponent pointLight2VelocityAcceleration{};
        pointLight2VelocityAcceleration.rotationVelocity = {.2f, .1f, .3f};

        pointLight2.velocityAcceleration = std::make_shared<VelocityAccelerationComponent>(pointLight2VelocityAcceleration);

        gameObjects.emplace(pointLight2.getId(), std::move(pointLight2));

        return gameObjects;
    }


    void Scenes::loadCoordinateSystem(LveDevice& device, LveGameObject::Map& gameObjects) {
        std::shared_ptr<LveModel> originModel = LveModel::createModelFromFile(device, "models/sphere.obj");
        std::shared_ptr<LveModel> arrowModel = LveModel::createModelFromFile(device, "models/cone.obj");

        auto origin = LveGameObject::createGameObject();
        origin.model = originModel;
        origin.transform.scale = {.05f, .05f, .05f};

        auto xArrow = LveGameObject::createGameObject();
        xArrow.model = arrowModel;
        xArrow.transform.scale = {.05f, .05f, .05f};
        xArrow.transform.translation = {0.1f, 0.f, 0.f};
        xArrow.transform.rotation = {0.f, 0.f, -glm::half_pi<float>()};

        auto yArrow = LveGameObject::createGameObject();
        yArrow.model = arrowModel;
        yArrow.transform.scale = {.05f, .05f, .05f};
        yArrow.transform.translation = {0.f, 0.1f, 0.f};
        yArrow.transform.rotation = {0.f, glm::half_pi<float>(), 0.f};

        auto zArrow = LveGameObject::createGameObject();
        zArrow.model = arrowModel;
        zArrow.transform.scale = {.05f, .05f, .05f};
        zArrow.transform.translation = {0.f, 0.f, 0.1f};
        zArrow.transform.rotation = {glm::half_pi<float>(), 0.f, 0.f};


        gameObjects.emplace(origin.getId(), std::move(origin));
        gameObjects.emplace(xArrow.getId(), std::move(xArrow));
        gameObjects.emplace(yArrow.getId(), std::move(yArrow));
        gameObjects.emplace(zArrow.getId(), std::move(zArrow));
    }
}
