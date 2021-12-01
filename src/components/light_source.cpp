// #include "light_source.hpp"
#include "../lve_game_object.hpp"

// std
#include <iostream>

namespace lve {

    // LightSource public
    LveGameObject LightSource::createDirectional(glm::vec3 direction, glm::vec4 color) {
        LightSource source{};
        source.kind = LightSourceKind::DirectionalLight;
        source.color = color;

        auto gameObject = createGameObjectWithLightSource(std::move(source));
        gameObject.transform.rotation = direction;

        return gameObject;
    }

    LveGameObject LightSource::createPoint(glm::vec3 position, float radius, glm::vec4 color) {
        LightSource source{};
        source.kind = PointLight;
        source.color = color;
        source.radius = radius;

        source.color.w = BRIGHTNESS_AT_RADIUS * radius * radius;

        auto gameObject = createGameObjectWithLightSource(std::move(source));
        gameObject.transform.translation = position;

        return gameObject;
    }

    // LightSource private
    LveGameObject LightSource::createGameObjectWithLightSource(LightSource source) {
        auto gameObject = LveGameObject::createGameObject();
        gameObject.lightSource = {std::move(source)};
        return gameObject;
    }

    // LightSourceDTO

    LightSourceDTO LightSourceDTO::fromGameObject(LveGameObject& obj) {
        if (!obj.lightSource.has_value()) {
            throw std::runtime_error("can't create LightSourceDTO form LveGameObject without a LightSource");
        }

        auto lightSource = obj.lightSource.value();

        LightSourceDTO dto{};
        dto.parameters.kind = lightSource.kind;
        dto.color = lightSource.color;

        switch (lightSource.kind)
        {
        case LightSourceKind::DirectionalLight:
            dto.value1 = {obj.transform.rotation, 0.f};
            break;

        case LightSourceKind::PointLight:
            dto.value1 = {obj.transform.translation, 0.f};
            break;
        
        default:
            throw std::runtime_error("can't convert some LightSourceKinds to LightSourceDTO");
            break;
        }

        return dto;
    }
}