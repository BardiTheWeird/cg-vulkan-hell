#pragma once

#include <glm/gtc/matrix_transform.hpp>

namespace lve {

    struct RotationHelpers
    {
        static glm::mat3 getRotationMatrix(glm::vec3 rotation) {
            const float c1 = glm::cos(rotation.y);
            const float c2 = glm::cos(rotation.x);
            const float c3 = glm::cos(rotation.z);
            const float s1 = glm::sin(rotation.y);
            const float s2 = glm::sin(rotation.x);
            const float s3 = glm::sin(rotation.z);

            return glm::mat3 {
                {
                    c1 * c3 + s1 * s2 * s3,
                    c2 * s3,
                    c1 * s2 * s3 - c3 * s1,
                },
                {
                    c3 * s1 * s2 - c1 * s3,
                    c2 * c3,
                    c1 * c3 * s2 + s1 * s3,
                },
                {
                    c2 * s1,
                    -s2,
                    c1 * c2,
                }
            };
        }
    };
    
}