#pragma once 

#include "../low-level/lve_pipeline.hpp"

// std
#include <memory>
#include <string>
#include <unordered_map>
#include <optional>

namespace lve {
    struct PipelineInfo {
        std::shared_ptr<LvePipeline> pipeline;
        VkPipelineLayout layout;
    };
    
    class PipelineManager {
    public:
        PipelineManager(const PipelineManager&) = delete;
        PipelineManager &operator=(const PipelineManager&) = delete;

        PipelineManager(LveDevice& device, VkRenderPass renderPass);
        ~PipelineManager();

        void addPipeline(std::string key, VkPipelineLayout layout, std::string vertShaderPath, std::string fragShaderPath);
        const std::optional<PipelineInfo> getPipeline(std::string key);

    private:
        using Map = std::unordered_map<std::string, PipelineInfo>;

        void destroyPipelineLayouts();

        Map pipelines{};

        LveDevice& lveDevice;
        VkRenderPass renderPass;
    };
}