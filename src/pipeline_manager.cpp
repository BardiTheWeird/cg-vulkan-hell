#include "pipeline_manager.hpp"

namespace lve {

    PipelineManager::PipelineManager(LveDevice& device, VkRenderPass _renderPass) 
        : lveDevice{device}, renderPass{_renderPass}  { }

    PipelineManager::~PipelineManager() {
        destroyPipelineLayouts();
    }

    void PipelineManager::addPipeline(std::string key, VkPipelineLayout layout, std::string vertShaderPath, std::string fragShaderPath) {
        PipelineInfo info{};
        info.layout = layout;

        PipelineConfigInfo pipelineConfig{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = info.layout;

        info.pipeline = std::make_shared<LvePipeline>(
            lveDevice,
            vertShaderPath,
            fragShaderPath,
            pipelineConfig);

        pipelines.insert_or_assign(key, std::move(info));
    }

    const std::optional<PipelineInfo> PipelineManager::getPipeline(std::string key) {
        if (pipelines.find(key) == pipelines.end()) {
            return std::nullopt;
        }
        
        return {pipelines.at(key)};
    }

    void PipelineManager::destroyPipelineLayouts() {
        for (auto& kv: pipelines) {
            auto& info = kv.second;
            vkDestroyPipelineLayout(lveDevice.device(), info.layout, nullptr);
        }
    }

}