#include "game_systems.hpp"
#include "utils/helpers.hpp"

namespace lve {

    void GameSystems::executeAll(std::vector<LveGameObject>& gameObjects, float frameTime) {
        move(gameObjects, frameTime);
        moveCircle(gameObjects, frameTime);
    }

    void GameSystems::move(std::vector<LveGameObject>& gameObjects, float frameTime) {
        for (auto& obj : gameObjects) {
            if (obj.velocityAcceleration == nullptr)
                continue;

            auto velocityAcceleration = obj.velocityAcceleration;
            velocityAcceleration->velocity += velocityAcceleration->acceleration * frameTime;
            velocityAcceleration->rotationVelocity += velocityAcceleration->rotationAcceleration * frameTime;

            obj.transform.shift(velocityAcceleration->velocity * frameTime);
            obj.transform.rotate(velocityAcceleration->rotationVelocity * frameTime);
        }
    }

    void GameSystems::moveCircle(std::vector<LveGameObject>& gameObjects, float frameTime) {
        for (auto& obj : gameObjects) {
            if (obj.circularMovement == nullptr)
                continue;

            auto circ = obj.circularMovement;
            auto center = circ->center;
            auto rotation = circ->rotation * circ->speed * frameTime;

            auto curPos = obj.transform.translation;
            obj.transform.translation = 
                RotationHelpers::getRotationMatrix(rotation) 
                * (curPos - center)
                + center;
        }
    }

}