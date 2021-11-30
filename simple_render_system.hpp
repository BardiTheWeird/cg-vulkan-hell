#pragma once

#include "lve_camera.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"
#include "lve_model.hpp"
#include "lve_game_object.hpp"
#include "lve_frame_info.hpp"

// std
#include <memory>
#include <vector>

namespace lve {
    
    class SimpleRenderSystem {
    public:
        SimpleRenderSystem(
            LveDevice& device, 
            VkRenderPass renderPass, 
            VkDescriptorSetLayout globalSetLayout, 
            VkDescriptorSetLayout textureSetLayout);
        ~SimpleRenderSystem();  

        SimpleRenderSystem(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem &operator=(const SimpleRenderSystem&) = delete;

        void renderGameObjects(FrameInfo& frameInfo);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout, VkDescriptorSetLayout textureSetLayout);
        void createPipeline(VkRenderPass& renderPass);

        LveDevice& lveDevice;

        std::unique_ptr<LvePipeline> coloredLvePipeline;
        VkPipelineLayout coloredPipelineLayout;
        std::unique_ptr<LvePipeline> texturedLvePipeline;
        VkPipelineLayout texturedPipelineLayout;
    };
}