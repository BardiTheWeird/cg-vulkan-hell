#include "game_system_manager.hpp"
#include "utils/helpers.hpp"
#include <iostream>

namespace lve {

    GameSystemManager::GameSystemManager(
        LveDevice& _lveDevice, 
        MaterialManager& _materialManager, 
        SimpleRenderSystem& _simpleRenderSystem) 
            : lveDevice{_lveDevice}, 
            materialManager{_materialManager}, 
            renderSystem{_simpleRenderSystem} {}

    void GameSystemManager::executeAll(FrameInfo& frameInfo) {
        move(frameInfo);
        moveCircle(frameInfo);

        updateMaterials(frameInfo);
        renderSystem.renderGameObjects(frameInfo);
    }

    void GameSystemManager::move(FrameInfo& frameInfo) {
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

    void GameSystemManager::moveCircle(FrameInfo& frameInfo) {
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

    void GameSystemManager::updateMaterials(FrameInfo& frameInfo) {
        for (auto& kv: frameInfo.gameObjects) {
            auto& obj = kv.second;
            if (obj.material.has_value() && obj.material.value().updatedThisFrame) {
                materialManager.updateMaterial(obj.material.value());
            }
        }
    }
}