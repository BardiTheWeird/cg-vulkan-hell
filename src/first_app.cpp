#include "first_app.hpp"

#include "low-level/simple_render_system.hpp"
#include "low-level/lve_buffer.hpp"
#include "keyboard_movement_controller.hpp"
#include "lve_camera.hpp"
#include "managers/pipeline_loader.hpp"
#include "scenes/scenes.hpp"

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
#include <string.h>

namespace lve
{
    FirstApp::FirstApp(int argc, char** argv)
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
            pipelineManager,
            modelManager
        );

        loadGameObjects(argc, argv);

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
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();
        while (!lveWindow.shouldClose()) {
            glfwPollEvents();

            // frameTime calculation
            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            // camera settings
            static bool isPerspective = true;
            if (cameraController.shouldSwitchProjection(lveWindow.getGlfwWindow()))
                isPerspective = !isPerspective;

            float aspect = lveRenderer.getAspectRatio();
            if (isPerspective) {
                camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 1000.f);
            }
            else {
                auto height = 8.f;
                auto halfHeight = height / 2.f;
                camera.setOrthographicProjection(-aspect * halfHeight, aspect * halfHeight, -halfHeight, halfHeight, -1.f, 1000.f);
            }
            camera.setViewYXZ(cameraObject.transform.translation, cameraObject.transform.rotation);
            cameraController.moveInPlaneXZ(lveWindow.getGlfwWindow(), frameTime, cameraObject);

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
                ubo.cameraPosition = {cameraObject.transform.translation, 0.f};

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

    void FirstApp::loadGameObjects(int argc, char** argv) {
        std::function<LveGameObject::Map (LveDevice&, LveGameObject&, TextureManager&, MaterialManager&, ModelManager&)> sceneLoaderFunction = nullptr;
        if (argc > 1) {
            if (!strcmp(argv[1], "lab1")) {
                sceneLoaderFunction = Scenes::loadSceneLab1;
            }
            else if (!strcmp(argv[1], "lab3")) {
                sceneLoaderFunction = Scenes::loadSceneLab3;
            }
        }

        if (sceneLoaderFunction == nullptr) {
            sceneLoaderFunction = Scenes::loadTestScene1;
        }
        gameObjects = sceneLoaderFunction(lveDevice, cameraObject, textureManager, materialManager, modelManager);
        std::cout << "loaded " << gameObjects.size() << " game objects\n";
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