#pragma once

#include "lve_game_object.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace lve {

    struct MoveEvent
    {
        LveGameObject::id_t objectId;
        glm::vec3 movement;
    };
    
}