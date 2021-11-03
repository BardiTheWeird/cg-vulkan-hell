#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "simple_render_system.hpp"
#include "lve_camera.hpp"
#include "lve_buffer.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <chrono>
#include <stdexcept>
#include <array>
#include <queue>
#include <iostream>

namespace lve
{

    struct GlobalUbo {
        glm::mat4 projectionView{1.f};
        glm::vec3 lightDirection = glm::normalize(glm::vec3{1.f, -3.f, -1.f});
    };

    FirstApp::FirstApp()
    {
        loadGameObjects();
    }

    FirstApp::~FirstApp() {}

    void FirstApp::run()
    {
        LveBuffer globalUbo{
            lveDevice,
            sizeof(GlobalUbo),
            LveSwapChain::MAX_FRAMES_IN_FLIGHT,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            lveDevice.properties.limits.minUniformBufferOffsetAlignment,
        };
        globalUbo.map();

        SimpleRenderSystem simpleRenderSystem{lveDevice, lveRenderer.getSwapChainRenderPass()};
        LveCamera camera{};

        auto viewerObject = LveGameObject::createGameObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!lveWindow.shouldClose())
        {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            auto frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(lveWindow.getGlfwWindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = lveRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, .1f, 50.f);


            if (auto commandBuffer = lveRenderer.beginFrame()) {
                int frameIndex = lveRenderer.getFrameIndex();
                FrameInfo frameInfo{
                    frameIndex, 
                    frameTime, 
                    commandBuffer,
                    camera
                };

                // update
                GlobalUbo ubo{};
                ubo.projectionView = camera.getProjectionMatrix() * camera.getViewMatrix();
                globalUbo.writeToIndex(&ubo, frameIndex);
                globalUbo.flushIndex(frameIndex);

                // render
                lveRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
                lveRenderer.endSwapChainRenderPass(commandBuffer);
                lveRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(lveDevice.device());
    }

    void FirstApp::loadGameObjects() {
        std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(lveDevice, "models/cube.obj");

        auto cube = LveGameObject::createGameObject();
        cube.model = lveModel;
        cube.transform.translation = {.0f, .0f, 5.5f};
        // cube.transform.scale = {.5f, .5f, .5f};

        gameObjects.push_back(std::move(cube));

        std::shared_ptr<LveModel> lveModel2 = LveModel::createModelFromFile(lveDevice, "models/colored_cube.obj");

        auto cube2 = LveGameObject::createGameObject();
        cube2.model = lveModel2;
        cube2.transform.translation = {1.f, 0.f, 2.5f};
        cube2.transform.rotation = {0.f, glm::quarter_pi<float>(), 0.f};
        cube2.transform.scale = {.5f, .5f, .5f};

        gameObjects.push_back(std::move(cube2));

        std::shared_ptr<LveModel> lveModel3 = LveModel::createModelFromFile(lveDevice, "models/smooth_vase.obj");

        auto smoothVase = LveGameObject::createGameObject();
        smoothVase.model = lveModel3;
        smoothVase.transform.translation = {-1.f, 0.f, 2.5f};
        // smoothVase.transform.scale = {.5f, .5f, .5f};

        gameObjects.push_back(std::move(smoothVase));

        std::shared_ptr<LveModel> lveModel4 = LveModel::createModelFromFile(lveDevice, "models/flat_vase.obj");

        auto flatVase = LveGameObject::createGameObject();
        flatVase.model = lveModel3;
        flatVase.transform.translation = {-.5f, 0.f, 2.5f};
        flatVase.transform.scale = {1.f, 1.f, 3.f};

        gameObjects.push_back(std::move(flatVase));
    }
}