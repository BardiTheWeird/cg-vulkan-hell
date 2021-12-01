#pragma once

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace lve {

    struct MaterialComponent {
        using id_t = unsigned int;

        bool updatedThisFrame;

        id_t material_id;
        glm::vec3 albedoMesh;
        float roughness;
        glm::vec3 emissivityMesh;
        glm::vec3 baseReflectance;
        float metallicCoefficient;
    };
}