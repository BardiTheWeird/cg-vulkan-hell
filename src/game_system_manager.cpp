#include "game_system_manager.hpp"
#include "utils/helpers.hpp"
#include <iostream>

namespace lve {
    using id_t = LveGameObject::id_t;

    GameSystemManager::GameSystemManager(
        LveDevice& _lveDevice, 
        MaterialManager& _materialManager, 
        SimpleRenderSystem& _simpleRenderSystem,
        LveGameObject::Map& gameObjects) 
            : lveDevice{_lveDevice}, 
            materialManager{_materialManager}, 
            renderSystem{_simpleRenderSystem} 
    {
        buildMovementDependencyMap(gameObjects);
    }

    void findAllFollowersRecursive(
        std::unordered_map<id_t, std::vector<id_t>>& movementDependencyMap, id_t leaderId, std::vector<id_t>& found) {
            found.push_back(leaderId);
            if (movementDependencyMap.find(leaderId) == movementDependencyMap.end()) {
                return;
            }

            for (auto followerId: movementDependencyMap.at(leaderId)) {
                findAllFollowersRecursive(movementDependencyMap, followerId, found);
            }
    }

    void findAllFollowers(
        std::unordered_map<id_t, std::vector<id_t>>& movementDependencyMap, id_t leaderId, std::vector<id_t>& found) {
        if (movementDependencyMap.find(leaderId) == movementDependencyMap.end()) {
            return;
        }

        for (auto followerId: movementDependencyMap.at(leaderId)) {
            findAllFollowersRecursive(movementDependencyMap, followerId, found);
        }       
    }

    void GameSystemManager::buildMovementDependencyMap(LveGameObject::Map& gameObjects) {
        // we place GOOD FUCKING FAITH in our game designers to prevent cyclic dependencies
        std::unordered_map<id_t, std::vector<id_t>> movementDependencyMap{};

        // build leader -> { follower1, follower2 } dependency map
        for (auto& kv: gameObjects) {
            auto& obj = kv.second;
            if (!obj.repeatMovement.has_value())
                continue;

            id_t leaderId = obj.repeatMovement.value().leaderId;
            id_t followerId = obj.getId();

            if (movementDependencyMap.find(leaderId) == movementDependencyMap.end()) {
                auto shallowFollowers = std::vector<id_t>{};
                shallowFollowers.push_back(followerId);
                movementDependencyMap.emplace(leaderId, shallowFollowers);
            }
            else {
                movementDependencyMap.at(leaderId).push_back(followerId);
            }
        }

        for (auto& kv: movementDependencyMap) {
            auto leaderId = kv.first;
            std::vector<id_t> flattenedDependencies{};
            findAllFollowers(movementDependencyMap, leaderId, flattenedDependencies);
            flattenedMovementDependencyMap.emplace(
                leaderId,
                std::move(flattenedDependencies)
            );
        }
    }


    void GameSystemManager::executeAll(FrameInfo& frameInfo) {
        enactVelocityAcceleration(frameInfo);
        moveCircle(frameInfo);

        enactRepeatMovement(frameInfo);
        applyMoveEvents(frameInfo);

        advanceOscillators(frameInfo);

        updateMaterials(frameInfo);
        renderSystem.renderGameObjects(frameInfo);
    }

    void GameSystemManager::enactVelocityAcceleration(FrameInfo& frameInfo) {
        float frameTime = frameInfo.frameTime;

        for (auto& kv : frameInfo.gameObjects) {
            auto& obj = kv.second;
            if (!obj.velocityAcceleration.has_value())
                continue;

            auto velocityAcceleration = obj.velocityAcceleration;
            velocityAcceleration->velocity += velocityAcceleration->acceleration * frameTime;
            velocityAcceleration->rotationVelocity += velocityAcceleration->rotationAcceleration * frameTime;

            // obj.transform.shift(velocityAcceleration->velocity * frameTime);
            obj.transform.rotate(velocityAcceleration->rotationVelocity * frameTime);
            moveEvents.push_back({
                obj.getId(),
                velocityAcceleration->velocity * frameTime
            });
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
            auto newPos = 
                RotationHelpers::getRotationMatrix(rotation) 
                * (curPos - center)
                + center;

            moveEvents.push_back({
                obj.getId(),
                newPos - curPos
            });
        }
    }

    void GameSystemManager::advanceOscillators(FrameInfo& frameInfo) {
        for (auto& kv: frameInfo.gameObjects) {
            auto& obj = kv.second;
            for (auto& oscillator: obj.oscillators) {
                oscillator->advance(frameInfo.frameTime, obj, moveEvents);
            }
        }
    }

    void GameSystemManager::enactRepeatMovement(FrameInfo& frameInfo) {
        // add move events based on the map
        for (auto& event: moveEvents) {
            if (flattenedMovementDependencyMap.find(event.objectId) == flattenedMovementDependencyMap.end()) {
                continue;
            }

            auto& followers = flattenedMovementDependencyMap.at(event.objectId);
            for (auto followerId: followers) {
                auto& obj = frameInfo.gameObjects.at(followerId);
                obj.transform.shift(event.movement);
            }
        }
    }

    void GameSystemManager::applyMoveEvents(FrameInfo& frameInfo) {
        while (moveEvents.size() > 0) {
            auto event = moveEvents.back();
            auto& obj = frameInfo.gameObjects.at(event.objectId);
            obj.transform.shift(event.movement);
            
            moveEvents.pop_back();
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