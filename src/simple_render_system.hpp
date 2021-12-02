#pragma once

#include "lve_camera.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"
#include "lve_model.hpp"
#include "lve_game_object.hpp"
#include "lve_frame_info.hpp"
#include "texture_manager.hpp"
#include "material_manager.hpp"
#include "pipeline_manager.hpp"

// std
#include <memory>
#include <vector>

namespace lve {
    
    class SimpleRenderSystem {
    public:
        SimpleRenderSystem(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem &operator=(const SimpleRenderSystem&) = delete;

        SimpleRenderSystem(
            LveDevice& device,
            TextureManager& _textureManager, 
            MaterialManager& materialManager,
            PipelineManager& pipelineManager);
        ~SimpleRenderSystem();  

        void renderGameObjects(FrameInfo& frameInfo);

    private:
        LveDevice& lveDevice;
        TextureManager& textureManager;
        MaterialManager& materialManager;
        PipelineManager& pipelineManager;

        PipelineInfo coloredPlainPipelineInfo;
        PipelineInfo coloredPbrPipelineInfo;
        PipelineInfo texturedPlainPipelineInfo;
        PipelineInfo texturedPbrPipelineInfo;
    };
}