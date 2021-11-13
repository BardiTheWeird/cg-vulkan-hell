#pragma once

// #include "../lve_game_object.hpp"

// std
#include <stdexcept>

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#define MIN_BRIGHTNESS 0.001f

namespace lve {

    class LveGameObject;

    struct LightSourceParameters {
        int32_t kind;
        float brightness;
        float misc2;
        float misc3;
    };

    struct LightSourceDTO
    {
        LightSourceParameters parameters;
        glm::vec4 value1; // direction OR point of origin

        static LightSourceDTO fromGameObject(LveGameObject& obj);
    };
    
    enum LightSourceKind : int32_t {
        DirectionalLight = 0, // direction is the transform's rotation
        PointLight = 1, // position is the transform's position
    };

    struct LightSource
    {
        bool turnedOn{true};
        float radius{3.f};
        LightSourceKind kind;

        static LveGameObject createDirectional(glm::vec3 direction);
        static LveGameObject createPoint(glm::vec3 position, float radius);

    private:
        static LveGameObject createGameObjectWithLightSource(LightSource source);
    };
}