#include "simple_render_system.hpp"
#include "push_constants.hpp"
#include "constants.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <array>
#include <queue>
#include <iostream>
#include <algorithm>

namespace lve
{
    SimpleRenderSystem::SimpleRenderSystem(
            LveDevice& device,
            TextureManager& _textureManager, 
            MaterialManager& _materialManager,
            PipelineManager& _pipelineManager,
            ModelManager& _modelManager) 
                : lveDevice{device}, 
                textureManager{_textureManager}, 
                materialManager{_materialManager},
                pipelineManager{_pipelineManager},
                modelManager{_modelManager}
            {
                using namespace constants::pipeline_keys;
                
                coloredPlainPipelineInfo = pipelineManager.getPipeline(colored_plain).value();
                coloredPbrPipelineInfo = pipelineManager.getPipeline(colored_pbr).value();
                texturedPlainPipelineInfo = pipelineManager.getPipeline(textured_plain).value();
                texturedPbrPipelineInfo = pipelineManager.getPipeline(textured_pbr).value();
            }

    SimpleRenderSystem::~SimpleRenderSystem() {
    }

    void SimpleRenderSystem::drawObject(LveGameObject& obj, FrameInfo& frameInfo, VkPipelineLayout pipelineLayout) {

        SimplePushConstantData push{};
        push.modelMatrix = obj.transform.mat4();
        push.normalMatrix = obj.transform.normalMatrix();

        vkCmdPushConstants(
            frameInfo.commandBuffer, 
            pipelineLayout, 
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(SimplePushConstantData),
            &push);

        auto model = modelManager.getModel(obj.modelKey.value()).value();
        model->bind(frameInfo.commandBuffer);
        model->draw(frameInfo.commandBuffer);
    }

    void SimpleRenderSystem::renderGameObjects(FrameInfo& frameInfo) {
        // common to all pipelines
        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            coloredPlainPipelineInfo.layout,
            0,
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr
        );

        // separate objects with and without textures
        std::vector<lve::LveGameObject*> coloredPlainObjects{};
        std::vector<lve::LveGameObject*> coloredPbrObjects{};
        std::vector<lve::LveGameObject*> texturedPlainObjects{};
        std::vector<lve::LveGameObject*> texturedPbrObjects{};

        for (auto& kv: frameInfo.gameObjects) {
            auto obj = &kv.second;
            if (!(obj->isVisible && obj->modelKey.has_value())) {
                continue;
            }

            if (obj->textureKey.has_value()) {
                if (obj->material.has_value())
                    texturedPbrObjects.push_back(obj);
                else
                    texturedPlainObjects.push_back(obj);
            }
            else {
                if (obj->material.has_value())
                    coloredPbrObjects.push_back(obj);
                else
                    coloredPlainObjects.push_back(obj);
            }
        }

        PipelineInfo pipelineInfo;

        // draw colored plain objects
        if (coloredPlainObjects.size() > 0) {
            pipelineInfo = coloredPlainPipelineInfo;
            pipelineInfo.pipeline->bind(frameInfo.commandBuffer);
            for (auto obj: coloredPlainObjects) {
                drawObject(*obj, frameInfo, pipelineInfo.layout);
            }    
        }

        // draw colored pbr objects
        if (coloredPbrObjects.size() > 0) {
            pipelineInfo = coloredPbrPipelineInfo;
            pipelineInfo.pipeline->bind(frameInfo.commandBuffer);
            for (auto obj: coloredPbrObjects) {
                auto materialDescriptorSet = materialManager.getDescriptorSet(obj->material.value().material_id);

                vkCmdBindDescriptorSets(
                    frameInfo.commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipelineInfo.layout,
                    1,
                    1,
                    &materialDescriptorSet,
                    0,
                    nullptr
                );

                drawObject(*obj, frameInfo, pipelineInfo.layout);
            }
        }

        if (texturedPlainObjects.size() > 0) {
            // draw objects with texture
            pipelineInfo = texturedPlainPipelineInfo;
            pipelineInfo.pipeline->bind(frameInfo.commandBuffer);
            for (auto obj: texturedPlainObjects) {
                std::string textureKey = obj->textureKey.value();
                VkDescriptorSet descriptorSet;
                if (!textureManager.getTextureDescriptorSet(textureKey, descriptorSet)) {
                    throw std::runtime_error("Couldn't find a texture with key " + textureKey);
                }

                vkCmdBindDescriptorSets(
                    frameInfo.commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipelineInfo.layout,
                    1,
                    1,
                    &descriptorSet,
                    0,
                    nullptr
                );

                drawObject(*obj, frameInfo, pipelineInfo.layout);
            }
        }

        if (texturedPbrObjects.size() > 0) {
            // draw objects with texture
            pipelineInfo = texturedPbrPipelineInfo;
            pipelineInfo.pipeline->bind(frameInfo.commandBuffer);
            for (auto obj: texturedPbrObjects) {
                std::string textureKey = obj->textureKey.value();
                VkDescriptorSet descriptorSet;
                if (!textureManager.getTextureDescriptorSet(textureKey, descriptorSet)) {
                    throw std::runtime_error("Couldn't find a texture with key " + textureKey);
                }

                vkCmdBindDescriptorSets(
                    frameInfo.commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipelineInfo.layout,
                    1,
                    1,
                    &descriptorSet,
                    0,
                    nullptr
                );

                auto materialDescriptorSet = materialManager.getDescriptorSet(obj->material.value().material_id);

                vkCmdBindDescriptorSets(
                    frameInfo.commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipelineInfo.layout,
                    2,
                    1,
                    &materialDescriptorSet,
                    0,
                    nullptr
                );

                drawObject(*obj, frameInfo, pipelineInfo.layout);
            }
        }
    }
}