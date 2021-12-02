#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "simple_render_system.hpp"
#include "lve_camera.hpp"
#include "lve_buffer.hpp"
#include "scenes/scenes.hpp"
#include "pipeline_loader.hpp"

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
    FirstApp::FirstApp()
    {
        globalSetLayout = LveDescriptorSetLayout::Builder(lveDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();

        globalPool = LveDescriptorPool::Builder(lveDevice)
            .setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();


        PipelineLoader::loadPipelines(
            lveDevice, 
            pipelineManager, 
            globalSetLayout->getDescriptorSetLayout(),
            textureManager.getTextureDescriptorSetLayout(),
            materialManager.getDescriptorSetLayout()
        );
        

        simpleRenderSystem = std::make_unique<SimpleRenderSystem>(
            lveDevice,
            textureManager, 
            materialManager,
            pipelineManager
        );

        loadGameObjects();

        gameSystemManager = std::make_unique<GameSystemManager>(
            lveDevice,
            materialManager,
            *simpleRenderSystem,
            gameObjects
        );
    }

    FirstApp::~FirstApp() {}

    void FirstApp::run() {
        std::vector<std::unique_ptr<LveBuffer>> uboBuffers(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++) {
            uboBuffers[i] = std::make_unique<LveBuffer>(
                lveDevice,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffers[i]->map();
        }

        std::vector<VkDescriptorSet> globalDescriptorSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            LveDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }

        LveCamera camera{};

        auto viewerObject = LveGameObject::createGameObject();
        viewerObject.transform.translation = {0.f, 0.f, -1.f};
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();
        while (!lveWindow.shouldClose()) {
            glfwPollEvents();

            // frameTime calculation
            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            // camera settings
            cameraController.moveInPlaneXZ(lveWindow.getGlfwWindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = lveRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 500.f);

            if (auto commandBuffer = lveRenderer.beginFrame()) {
                int frameIndex = lveRenderer.getFrameIndex();
                FrameInfo frameInfo{
                    frameIndex, 
                    frameTime, 
                    commandBuffer, 
                    camera, 
                    globalDescriptorSets[frameIndex],
                    gameObjects
                };

                // update
                GlobalUbo ubo{};
                ubo.projectionView = camera.getProjectionMatrix() * camera.getViewMatrix();
                ubo.cameraPosition = {viewerObject.transform.translation, 0.f};

                getLightSources(ubo);

                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // render
                lveRenderer.beginSwapChainRenderPass(commandBuffer);
                gameSystemManager->executeAll(frameInfo);
                lveRenderer.endSwapChainRenderPass(commandBuffer);
                lveRenderer.endFrame();
            }
    }

    vkDeviceWaitIdle(lveDevice.device());
    }

    void FirstApp::loadGameObjects() {
        gameObjects = std::move(Scenes::loadTestScene1(lveDevice, textureManager, materialManager));
    }

    void FirstApp::getLightSources(GlobalUbo& ubo) {
        int index{0};
        for (auto& kv : gameObjects) {
            auto& obj = kv.second;
            auto lightSource = obj.lightSource;
            if (!(lightSource.has_value() && lightSource.value().turnedOn)) {
                continue;
            }

            ubo.lightSources[index] = LightSourceDTO::fromGameObject(obj);
            if (++index >= MAX_LIGHT_SOURCES) {
                break;
            }
        }
        ubo.parameters.lightSourceCount = std::move(index);
    }
}