#pragma once

#include "lve_window.hpp"
#include "lve_device.hpp"
#include "lve_model.hpp"
#include "lve_game_object.hpp"
#include "lve_renderer.hpp"
#include "lve_descriptors.hpp"

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
        glm::vec4 ambientColor{1.f, 1.f, 1.f, .02f};
        GlobalUboParameters parameters;
        LightSourceDTO lightSources[MAX_LIGHT_SOURCES];
    };
    
    class FirstApp {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        FirstApp();
        ~FirstApp();

        FirstApp(const FirstApp&) = delete;
        FirstApp &operator=(const FirstApp&) = delete;


        void run();

    private:
        void loadGameObjects();
        void getLightSources(GlobalUbo& ubo);
        void moveOnSchedule(float frameTime);

        LveWindow lveWindow{WIDTH, HEIGHT, "Hello, Vulkan!"};
        LveDevice lveDevice{lveWindow};
        LveRenderer lveRenderer{lveWindow, lveDevice};

        std::unique_ptr<LveDescriptorPool> globalPool;
        std::unique_ptr<LveDescriptorPool> textureDescriptorPool;
        LveGameObject::Map gameObjects;
    };
}