#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// std
#include <string>

namespace lve {
    class LveWindow {

        public:
            LveWindow(int w, int h, std::string name);
            ~LveWindow();

            LveWindow(const LveWindow&) = delete;
            LveWindow &operator=(const LveWindow&) = delete;

            bool shouldClose();
            VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; } 

            void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
            bool wasWindowResized() { return frameBufferResized; }
            void resetWindowResizedFlag() { frameBufferResized = false; }
            GLFWwindow* getGlfwWindow() { return window; }

        private:
            GLFWwindow* window;
            static void frameBufferResizeCallback(GLFWwindow* window, int width, int height);

            int width;
            int height;
            bool frameBufferResized = false;

            std::string windowName;

            void initWindow();
    };
}