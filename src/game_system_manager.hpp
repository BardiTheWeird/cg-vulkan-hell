#pragma once

#include "lve_frame_info.hpp"
#include "material_manager.hpp"
#include "simple_render_system.hpp"
#include "move_event.hpp"

// std
#include <vector>
#include <unordered_set>

namespace lve {

    class GameSystemManager {
    public:
        GameSystemManager(const GameSystemManager&) = delete;
        GameSystemManager &operator=(const GameSystemManager&) = delete;

        GameSystemManager(LveDevice& lveDevice, MaterialManager& materialManager, SimpleRenderSystem& simpleRenderSystem, LveGameObject::Map& gameObjects);

        void executeAll(FrameInfo& frameInfo);

    private:
        LveDevice& lveDevice;
        MaterialManager& materialManager;
        SimpleRenderSystem& renderSystem;

        std::vector<MoveEvent> moveEvents{};
        std::unordered_map<LveGameObject::id_t, std::vector<LveGameObject::id_t>> flattenedMovementDependencyMap{};

        void buildMovementDependencyMap(LveGameObject::Map& gameObjects);

        void enactVelocityAcceleration(FrameInfo& frameInfo);
        void moveCircle(FrameInfo& frameInfo);
        void moveCircleAroundObject(FrameInfo& frameInfo);
        
        void advanceOscillators(FrameInfo& frameInfo);

        void enactRepeatMovement(FrameInfo& frameInfo);
        void applyMoveEvents(FrameInfo& frameInfo);
        
        void updateMaterials(FrameInfo& frameInfo);
    };
}