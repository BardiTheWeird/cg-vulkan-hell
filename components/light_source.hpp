#pragma once

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace lve {

    struct LightSourceParameters {
        int32_t kind;
        float par1;
        float misc2;
        float misc3;
    };

    struct LightSourceDTO
    {
        LightSourceParameters parameters;
        glm::vec4 value1; // direction OR point of origin
    };
    
    enum LightSourceKind : int32_t {
        DirectionalLight = 0,
        PointLight = 1,
    };

    struct LightSource
    {
        bool turnedOn{true};
        float brightness{1000.f};
        LightSourceKind kind;
        glm::vec3 value1; // point of origin OR direction

        LightSourceDTO toDTO() const {
            return LightSourceDTO {
                {kind, brightness, 0, 0},
                {value1, 0}
            };
        }

        static LightSource createDirectional(glm::vec3 direction) {
            LightSource source{};
            source.kind = LightSourceKind::DirectionalLight;
            source.value1 = glm::normalize(direction);

            return source;
        }

        static LightSource createPoint(glm::vec3 position, float brightness) {
            LightSource source{};
            source.kind = PointLight;
            source.brightness = brightness;
            source.value1 = position;

            return source;
        }
    };
}