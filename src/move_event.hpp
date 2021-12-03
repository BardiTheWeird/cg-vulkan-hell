#pragma once

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace lve {

    struct MoveEvent
    {
        unsigned int objectId;
        glm::vec3 movement;
    };
    
}