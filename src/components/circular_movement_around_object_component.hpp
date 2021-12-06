#pragma once

// libs
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace lve {

    struct CircularMovementAroundObjectComponent {
        unsigned int objectId;
        glm::vec3 rotation = glm::normalize(glm::vec3{glm::half_pi<float>(), glm::half_pi<float>(), glm::half_pi<float>()});
        float speed{1.f};
    };
}