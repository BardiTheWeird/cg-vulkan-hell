#include "game_systems.hpp"
#include "utils/helpers.hpp"

namespace lve {

    void GameSystems::executeAll(GameSystemInfo& gameSystemInfo) {
        move(gameSystemInfo);
        moveCircle(gameSystemInfo);
    }

    void GameSystems::move(GameSystemInfo& gameSystemInfo) {
        float frameTime = gameSystemInfo.frameTime;

        for (auto& kv : gameSystemInfo.gameObjects) {
            auto& obj = kv.second;
            if (obj.velocityAcceleration == nullptr)
                continue;

            auto velocityAcceleration = obj.velocityAcceleration;
            velocityAcceleration->velocity += velocityAcceleration->acceleration * frameTime;
            velocityAcceleration->rotationVelocity += velocityAcceleration->rotationAcceleration * frameTime;

            obj.transform.shift(velocityAcceleration->velocity * frameTime);
            obj.transform.rotate(velocityAcceleration->rotationVelocity * frameTime);
        }
    }

    void GameSystems::moveCircle(GameSystemInfo& gameSystemInfo) {
        for (auto& kv : gameSystemInfo.gameObjects) {
            auto& obj = kv.second;
            if (obj.circularMovement == nullptr)
                continue;

            auto circ = obj.circularMovement;
            auto center = circ->center;
            auto rotation = circ->rotation * circ->speed * gameSystemInfo.frameTime;

            auto curPos = obj.transform.translation;
            obj.transform.translation = 
                RotationHelpers::getRotationMatrix(rotation) 
                * (curPos - center)
                + center;
        }
    }

}