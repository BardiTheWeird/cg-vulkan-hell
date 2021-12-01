#include "pipeline_loader.hpp"
#include "push_constants.hpp"
#include "constants.hpp"

// std
#include <stdexcept>

namespace lve {

    void PipelineLoader::loadPipelines(
            LveDevice& lveDevice, 
            PipelineManager& pipelineManager,
            VkDescriptorSetLayout globalSetLayout,
            VkDescriptorSetLayout textureSetLayout,
            VkDescriptorSetLayout materialSetLayout) {

        // colored plain pipeline layout
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        auto createPipelineLayout = [&](std::vector<VkDescriptorSetLayout>& descriptorSetLayouts) {
            VkPipelineLayoutCreateInfo coloredPlainLayoutInfo{};
            coloredPlainLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            coloredPlainLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
            coloredPlainLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
            coloredPlainLayoutInfo.pushConstantRangeCount = 1;
            coloredPlainLayoutInfo.pPushConstantRanges = &pushConstantRange;

            VkPipelineLayout layout;
            if (vkCreatePipelineLayout(lveDevice.device(), &coloredPlainLayoutInfo, nullptr, &layout) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create colored_plain_layout");
            }

            return layout;
        };

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts2{globalSetLayout, textureSetLayout};
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts3{globalSetLayout, textureSetLayout, materialSetLayout};

        VkPipelineLayout coloredPlainPipelineLayout = createPipelineLayout(descriptorSetLayouts);
        VkPipelineLayout coloredPbrPipelineLayout = createPipelineLayout(descriptorSetLayouts3);
        VkPipelineLayout texturedPlainPipelineLayout = createPipelineLayout(descriptorSetLayouts2);
        VkPipelineLayout texturedPbrPipelineLayout = createPipelineLayout(descriptorSetLayouts3);


        pipelineManager.addPipeline(constants::pipeline_keys::colored_plain, coloredPlainPipelineLayout,
            "col_shader.vert.spv", "col_shader.frag.spv");
        // pipelineManager.addPipeline(constants::pipelines::colored_pbr, coloredPbrPipelineLayout,
            // "col_pbr_shader.vert.spv", "col_pbr_shader.frag.spv");
        pipelineManager.addPipeline(constants::pipeline_keys::textured_plain, texturedPlainPipelineLayout,
            "tex_shader.vert.spv", "tex_shader.frag.spv");
        // pipelineManager.addPipeline(constants::pipelines::textured_pbr, texturedPbrPipelineLayout,
            // "tex_pbr_shader.vert.spv", "tex_pbr_shader.frag.spv");
    }
}