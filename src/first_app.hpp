#pragma once

#include "low-level/lve_window.hpp"
#include "low-level/lve_device.hpp"
#include "low-level/lve_model.hpp"
#include "lve_game_object.hpp"
#include "low-level/lve_renderer.hpp"
#include "low-level/lve_descriptors.hpp"
#include "managers/texture_manager.hpp"
#include "managers/material_manager.hpp"
#include "managers/pipeline_manager.hpp"
#include "managers/game_system_manager.hpp"
#include "managers/model_manager.hpp"

#define MAX_LIGHT_SOURCES 128

// std
#include <memory>
#include <vector>

namespace lve {

    struct GlobalUboParameters {
        int lightSourceCount;
        int unused1;
        int unused2;
        int unused3;
    };

    struct GlobalUbo {
        glm::mat4 projectionView{1.f};
        glm::vec4 ambientColor{1.f, 1.f, 1.f, .001f};
        glm::vec4 cameraPosition{};
        GlobalUboParameters parameters;
        LightSourceDTO lightSources[MAX_LIGHT_SOURCES];
    };
    
    class FirstApp {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        FirstApp(int argc, char** argv);
        ~FirstApp();

        FirstApp(const FirstApp&) = delete;
        FirstApp &operator=(const FirstApp&) = delete;


        void run();

    private:
        void loadGameObjects(int argc, char** argv);
        void getLightSources(GlobalUbo& ubo);
        void moveOnSchedule(float frameTime);

        LveWindow lveWindow{WIDTH, HEIGHT, "Hello, Vulkan!"};
        LveDevice lveDevice{lveWindow};
        LveRenderer lveRenderer{lveWindow, lveDevice};
        TextureManager textureManager{lveDevice};
        MaterialManager materialManager{lveDevice};
        PipelineManager pipelineManager{lveDevice, lveRenderer.getSwapChainRenderPass()};
        ModelManager modelManager{lveDevice};

        std::unique_ptr<SimpleRenderSystem> simpleRenderSystem;
        std::unique_ptr<GameSystemManager> gameSystemManager;

        std::unique_ptr<LveDescriptorPool> globalPool;
        std::unique_ptr<LveDescriptorSetLayout> globalSetLayout;

        LveGameObject::Map gameObjects;
        LveGameObject cameraObject = LveGameObject::createGameObject();
    };
}