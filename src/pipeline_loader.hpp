#pragma once

#include "pipeline_manager.hpp"

namespace lve {
    
    class PipelineLoader {
    public:
        static void loadPipelines(
            LveDevice& lveDevice, 
            PipelineManager& pipelineManager,
            VkDescriptorSetLayout globalSetLayout,
            VkDescriptorSetLayout textureSetLayout,
            VkDescriptorSetLayout materialSetLayout
        );
    };
}