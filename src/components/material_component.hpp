#pragma once

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace lve {

    struct MaterialComponent {
        using id_t = unsigned int;

        id_t material_id;
        glm::vec3 albedoMesh{2.0};
        float roughness{0.1};
        glm::vec4 emissivityMesh{0.0};
        glm::vec3 baseReflectance{0.3};
        float metallicCoefficient{0.0};

        bool updatedThisFrame{true};
    };
}