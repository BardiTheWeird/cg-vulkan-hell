#include "simple_render_system.hpp"

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
    struct SimplePushConstantData {
        glm::mat4 modelMatrix{1.f};
        glm::mat4 normalMatrix{1.f};
    };

    SimpleRenderSystem::SimpleRenderSystem(
        LveDevice& device, 
        VkRenderPass renderPass, 
        VkDescriptorSetLayout globalSetLayout, 
        VkDescriptorSetLayout textureSetLayout) : lveDevice{device}
    {
        createPipelineLayout(globalSetLayout, Texture::getDescriptorSetLayout(lveDevice));
        createPipeline(renderPass);
    }

    SimpleRenderSystem::~SimpleRenderSystem()
    {
        vkDestroyPipelineLayout(lveDevice.device(), coloredPipelineLayout, nullptr);
        vkDestroyPipelineLayout(lveDevice.device(), texturedPipelineLayout, nullptr);
    }

    void SimpleRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout, VkDescriptorSetLayout textureSetLayout)
    {
        // colored pipeline layout
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &coloredPipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create coloredPipelineLayout");
        }

        // textured pipeline layout
        // VkPushConstantRange pushConstantRange{};
        // pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        // pushConstantRange.offset = 0;
        // pushConstantRange.size = sizeof(SimplePushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts2{globalSetLayout, textureSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo2{};
        pipelineLayoutInfo2.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo2.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts2.size());
        pipelineLayoutInfo2.pSetLayouts = descriptorSetLayouts2.data();
        pipelineLayoutInfo2.pushConstantRangeCount = 1;
        pipelineLayoutInfo2.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo2, nullptr, &texturedPipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create pipelineLayout");
        }
    }

    void SimpleRenderSystem::createPipeline(VkRenderPass& renderPass)
    {
        assert(coloredPipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = coloredPipelineLayout;

        coloredLvePipeline = std::make_unique<LvePipeline>(
            lveDevice,
            "shaders/col_shader.vert.spv",
            "shaders/col_shader.frag.spv",
            pipelineConfig);
        
        PipelineConfigInfo pipelineConfig2{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfig2);
        pipelineConfig2.renderPass = renderPass;
        pipelineConfig2.pipelineLayout = texturedPipelineLayout;

        texturedLvePipeline = std::make_unique<LvePipeline>(
            lveDevice,
            "shaders/tex_shader.vert.spv",
            "shaders/tex_shader.frag.spv",
            pipelineConfig2);
    }

    void drawObject(LveGameObject& obj, FrameInfo& frameInfo, VkPipelineLayout pipelineLayout) {

        std::cout << "trying to render" << std::endl;
        
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

        obj.model->bind(frameInfo.commandBuffer);
        obj.model->draw(frameInfo.commandBuffer);
    }

    void SimpleRenderSystem::renderGameObjects(FrameInfo& frameInfo) {
        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            coloredPipelineLayout,
            0,
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr
        );

        // separate objects with and without textures
        std::vector<lve::LveGameObject*> texturedObjects{};
        std::vector<lve::LveGameObject*> coloredObjects{};

        for (auto& kv: frameInfo.gameObjects) {
            auto obj = &kv.second;
            if (obj->isVisible && obj->model == nullptr) {
                continue;
            }

            if (obj->texture == nullptr) {
                coloredObjects.push_back(obj);
            }
            else {
                texturedObjects.push_back(obj);
            }
        }

        // draw objects with texture
        texturedLvePipeline->bind(frameInfo.commandBuffer);
        for (auto obj: texturedObjects) {
            
            auto descriptorSet = obj->texture->getDescriptorSet();
            vkCmdBindDescriptorSets(
                frameInfo.commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                texturedPipelineLayout,
                1,
                1,
                &descriptorSet,
                0,
                nullptr
            );

            drawObject(*obj, frameInfo, texturedPipelineLayout);
        }

        // draw objects without texture
        coloredLvePipeline->bind(frameInfo.commandBuffer);
        for (auto obj: coloredObjects) {
            drawObject(*obj, frameInfo, coloredPipelineLayout);
        }
    }
}