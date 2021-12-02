#pragma once

#include "lve_frame_info.hpp"
#include "material_manager.hpp"
#include "simple_render_system.hpp"
#include "move_event.hpp"

// std
#include <vector>

namespace lve {

    class GameSystemManager {
    public:
        GameSystemManager(const GameSystemManager&) = delete;
        GameSystemManager &operator=(const GameSystemManager&) = delete;

        GameSystemManager(LveDevice& lveDevice, MaterialManager& materialManager, SimpleRenderSystem& simpleRenderSystem);

        void executeAll(FrameInfo& frameInfo);

    private:
        LveDevice& lveDevice;
        MaterialManager& materialManager;
        SimpleRenderSystem& renderSystem;

        std::vector<MoveEvent> moveEvents{};

        void enactVelocityAcceleration(FrameInfo& frameInfo);
        void moveCircle(FrameInfo& frameInfo);
        void applyMoveEvents(FrameInfo& frameInfo);
        void updateMaterials(FrameInfo& frameInfo);
    };
}