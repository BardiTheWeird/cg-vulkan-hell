#pragma once

// #include "../lve_game_object.hpp"

// std
#include <stdexcept>

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#define BRIGHTNESS_AT_RADIUS 0.5f

namespace lve {

    class LveGameObject;

    struct LightSourceParameters {
        int32_t kind;
        float misc1;
        float misc2;
        float misc3;
    };

    struct LightSourceDTO
    {
        LightSourceParameters parameters;
        glm::vec4 color; // {r, g, b, intensity}
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
        glm::vec4 color{1.f};
        float radius{3.f};
        LightSourceKind kind;

        static LveGameObject createDirectional(glm::vec3 direction, glm::vec4 color = glm::vec4{1.f, 1.f, 1.f, 1.f});
        static LveGameObject createPoint(glm::vec3 position, float radius, glm::vec4 color = glm::vec4{1.f, 1.f, 1.f, 1.f});

    private:
        static LveGameObject createGameObjectWithLightSource(LightSource source);
    };
}