#pragma once

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace lve {

    struct LightSourceParameters {
        int32_t kind;
        int32_t misc1;
        int32_t misc2;
        int32_t misc3;
    };

    struct LightSourceDTO
    {
        LightSourceParameters parameters;
        glm::vec4 value1; // point of origin OR direction
    };
    
    enum LightSourceKind : int32_t {
        DirectionalLight = 0,
        PointLight = 1,
    };

    struct LightSource
    {
        bool turnedOn;
        LightSourceKind kind;
        glm::vec3 value1; // point of origin OR direction

        LightSourceDTO toDTO() const {
            return LightSourceDTO {
                {kind, 0, 0, 0},
                {value1, 0}
            };
        }
    };
}