#include "game_systems.hpp"
#include "utils/helpers.hpp"

namespace lve {

    void GameSystems::executeAll(FrameInfo& frameInfo) {
        move(frameInfo);
        moveCircle(frameInfo);
    }

    void GameSystems::move(FrameInfo& frameInfo) {
        float frameTime = frameInfo.frameTime;

        for (auto& kv : frameInfo.gameObjects) {
            auto& obj = kv.second;
            if (!obj.velocityAcceleration.has_value())
                continue;

            auto velocityAcceleration = obj.velocityAcceleration;
            velocityAcceleration->velocity += velocityAcceleration->acceleration * frameTime;
            velocityAcceleration->rotationVelocity += velocityAcceleration->rotationAcceleration * frameTime;

            obj.transform.shift(velocityAcceleration->velocity * frameTime);
            obj.transform.rotate(velocityAcceleration->rotationVelocity * frameTime);
        }
    }

    void GameSystems::moveCircle(FrameInfo& frameInfo) {
        for (auto& kv : frameInfo.gameObjects) {
            auto& obj = kv.second;
            if (!obj.circularMovement.has_value())
                continue;

            auto circ = obj.circularMovement;
            auto center = circ->center;
            auto rotation = circ->rotation * circ->speed * frameInfo.frameTime;

            auto curPos = obj.transform.translation;
            obj.transform.translation = 
                RotationHelpers::getRotationMatrix(rotation) 
                * (curPos - center)
                + center;
        }
    }

}