#pragma once

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace lve {

    class LveCamera
    {
    public:
        // LveCamera(/* args */);
        // ~LveCamera();

        void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
        void setPerspectiveProjection(float fovy, float aspect, float near, float far);

        const glm::mat4& getProjectionMatrix() const { return projectionMatrix; }

    private:
        glm::mat4 projectionMatrix{1.f};
    };   
}