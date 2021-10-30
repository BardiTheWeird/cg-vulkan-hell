#include "first_app.hpp"

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

namespace lve
{

    struct SimplePushConstantData {
        glm::mat2 transform{1.f};
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    FirstApp::FirstApp()
    {
        loadGameObjects();
        createPipelineLayout();
        recreateSwapChain();
        createCommandBuffers();
    }

    FirstApp::~FirstApp()
    {
        vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
    }

    void FirstApp::run()
    {
        while (!lveWindow.shouldClose())
        {
            glfwPollEvents();
            drawFrame();
        }

        vkDeviceWaitIdle(lveDevice.device());
    }

    struct Triangle {
        LveModel::Vertex top;
        LveModel::Vertex left;
        LveModel::Vertex right;

        Triangle(LveModel::Vertex _top, LveModel::Vertex _left, LveModel::Vertex _right)
            : top{_top}, left{_left}, right{_right} {}
    };

    std::vector<LveModel::Vertex> constructSierpinskiTriangle(uint iterations) {
        std::queue<Triangle> triangles{};
        triangles.push({
            {{0.0f, -0.95f}},
            {{-0.95f, 0.95f}},
            {{0.95f, 0.95f}}
        });

        for (int i = 0; i < iterations; i++) {
            uint triangles_to_pop = pow(3, i);
            for (auto _j = 0; _j < triangles_to_pop; _j++) {
                auto cur = triangles.front();
                triangles.pop();

                auto t = cur.top;
                auto l = cur.left;
                auto r = cur.right;

                auto li_x = (t.position[0] + l.position[0]) / 2;
                auto li_y = (l.position[1] + t.position[1]) / 2;

                auto ri_x = (r.position[0] + t.position[0]) / 2;
                auto ri_y = (r.position[1] + t.position[1]) / 2;

                auto bi_x = t.position[0];
                auto bi_y = r.position[1];

                LveModel::Vertex li{{li_x, li_y}}; 
                LveModel::Vertex ri{{ri_x, ri_y}};
                LveModel::Vertex bi{{bi_x, bi_y}};

                Triangle tr_top{t, li, ri};
                Triangle tr_left{li, l, bi};
                Triangle tr_right{ri, bi, r};

                triangles.push(tr_right);
                triangles.push(tr_top);
                triangles.push(tr_left);
            }
        }


        std::vector<LveModel::Vertex> v_out{};

        // background
        // glm::vec3 black{0.0f, 0.0f, 0.1f};
        // glm::vec3 gray{0.5f, 0.5f, 0.5f};
        // glm::vec3 white(0.95f, 0.95f, 0.75f);

        // v_out.push_back({{-1, -1}, gray});
        // v_out.push_back({{-1, 1}, black});
        // v_out.push_back({{1, 1}, gray});

        // v_out.push_back({{-1, -1}, gray});
        // v_out.push_back({{1, 1}, gray});
        // v_out.push_back({{1, -1}, white});

        while (!triangles.empty()) {
            auto tr = triangles.front();

            tr.top.color = {1.0f, 0.0f, 0.0f};
            tr.left.color = {0.0f, 1.0f, 0.0f};
            tr.right.color = {0.0f, 0.0f, 1.0f};

            v_out.push_back(tr.top);
            v_out.push_back(tr.left);
            v_out.push_back(tr.right);

            triangles.pop();
        }

        return v_out;
    }

    void FirstApp::loadGameObjects() {
        std::vector<LveModel::Vertex> vertices{
            {{0.0f, -0.5f}},
            {{0.5f, 0.5f}},
            {{-0.5f, 0.5f}}
        };
        
        auto lveModel = std::make_shared<LveModel>(lveDevice, vertices);
        auto triangle = LveGameObject::CreateGameObject();
        triangle.model = lveModel;
        triangle.color = {.1f, .8f, .1f};

        triangle.transform2d.translation.x = .2f;
        triangle.transform2d.rotation = 0.5 * glm::pi<float>();

        gameObjects.push_back(std::move(triangle)); 
    }

    void FirstApp::createPipelineLayout()
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create pipelineLayout");
        }
    }

    void FirstApp::createPipeline()
    {
        assert(lveSwapChain != nullptr && "Cannot create pipeline before swap chain");
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = lveSwapChain->getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;

        lvePipeline = std::make_unique<LvePipeline>(
            lveDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig);
    }

    void FirstApp::recreateSwapChain() {
        auto extent = lveWindow.getExtent();
        while (extent.width == 0 || extent.height == 0)
        {
            extent = lveWindow.getExtent();
            glfwWaitEvents();
        }
        
        vkDeviceWaitIdle(lveDevice.device());

        if (lveSwapChain == nullptr) {
            lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent);
        } else {
            lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent, std::move(lveSwapChain));
            if (lveSwapChain->imageCount() != commandBuffers.size()) {
                freeCommandBuffer();
                createCommandBuffers();
            }
        }
        
        createPipeline();
    }

    void FirstApp::createCommandBuffers()
    {
        commandBuffers.resize(lveSwapChain->imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = lveDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(lveDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create command buffers");
        }

        for (int i = 0; i < commandBuffers.size(); i++)
        {

        }
    }

    void FirstApp::freeCommandBuffer() {
        vkFreeCommandBuffers(
            lveDevice.device(), 
            lveDevice.getCommandPool(), 
            static_cast<uint32_t>(commandBuffers.size()), 
            commandBuffers.data());

        commandBuffers.clear();
    }

    void FirstApp::recordCommandBuffer(int imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = lveSwapChain->getRenderPass();
        renderPassInfo.framebuffer = lveSwapChain->getFrameBuffer(imageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = lveSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(lveSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(lveSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, lveSwapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

        renderGameObjects(commandBuffers[imageIndex]);

        vkCmdEndRenderPass(commandBuffers[imageIndex]);

        if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer");
        }
    }

    void FirstApp::renderGameObjects(VkCommandBuffer commandBuffer) {
        lvePipeline->bind(commandBuffer);

        for (auto& obj: gameObjects) {
            obj.transform2d.rotation = glm::mod(obj.transform2d.rotation + 0.01f, glm::two_pi<float>());

            SimplePushConstantData push{};
            push.offset = obj.transform2d.translation;
            push.color = obj.color;
            obj.transform2d.scale = {1.5f, 0.5f};


            push.transform = obj.transform2d.mat2();

            vkCmdPushConstants(
                commandBuffer, 
                pipelineLayout, 
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push);

            obj.model->bind(commandBuffer);
            obj.model->draw(commandBuffer);
        }
    }

    void FirstApp::drawFrame()
    {
        uint32_t imageIndex;
        auto result = lveSwapChain->acquireNextImage(&imageIndex);
    
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS || result != VK_SUBOPTIMAL_KHR)
        {
            std::runtime_error("failed to acquire the next swapChain image");
        }

        recordCommandBuffer(imageIndex);
        result = lveSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || lveWindow.wasWindowResized()) {
            lveWindow.resetWindowResizedFlag();
            recreateSwapChain();
            return;
        }
        
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swapChain image");
        }
    }

}