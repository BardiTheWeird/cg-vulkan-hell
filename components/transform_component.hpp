#pragma once

// libs
#include <glm/gtc/matrix_transform.hpp>

namespace lve {

    struct TransformComponent
        {
            glm::vec3 translation{};
            glm::vec3 scale{1.f, 1.f, 1.f};
            glm::vec3 rotation{};

            void shift(glm::vec3 shift);
            void rotate(glm::vec3 rotationalShift);

            glm::mat4 mat4();
            glm::mat3 normalMatrix();
        };
}