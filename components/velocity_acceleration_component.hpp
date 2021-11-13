#pragma once

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace lve {
    
    struct VelocityAccelerationComponent {
        glm::vec3 velocity{0.f};
        glm::vec3 acceleration{0.f};

        glm::vec3 rotationVelocity{0.f};
        glm::vec3 rotationAcceleration{0.f};
    };
}