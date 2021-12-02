#pragma once

#include "lve_frame_info.hpp"
#include "material_manager.hpp"
#include "simple_render_system.hpp"

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

        void move(FrameInfo& frameInfo);
        void moveCircle(FrameInfo& frameInfo);
        void updateMaterials(FrameInfo& frameInfo);
    };
}