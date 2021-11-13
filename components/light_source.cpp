// #include "light_source.hpp"
#include "../lve_game_object.hpp"

// std
#include <iostream>

namespace lve {

    // LightSource public
    LveGameObject LightSource::createDirectional(glm::vec3 direction) {
        LightSource source{};
        source.kind = LightSourceKind::DirectionalLight;

        auto gameObject = createGameObjectWithLightSource(std::move(source));
        gameObject.transform.rotation = direction;

        return gameObject;
    }

    LveGameObject LightSource::createPoint(glm::vec3 position, float radius) {
        LightSource source{};
        source.kind = PointLight;
        source.radius = radius;

        auto gameObject = createGameObjectWithLightSource(std::move(source));
        gameObject.transform.translation = position;

        return gameObject;
    }

    // LightSource private
    LveGameObject LightSource::createGameObjectWithLightSource(LightSource source) {
        auto gameObject = LveGameObject::createGameObject();
        gameObject.lightSource = std::make_shared<LightSource>(std::move(source));
        return gameObject;
    }

    // LightSourceDTO

    LightSourceDTO LightSourceDTO::fromGameObject(LveGameObject& obj) {
        if (obj.lightSource == nullptr) {
            throw std::runtime_error("can't create LightSourceDTO form LveGameObject without a LightSource");
        }

        auto lightSource = obj.lightSource;

        LightSourceDTO dto{};
        dto.parameters.kind = lightSource->kind;

        switch (lightSource->kind)
        {
        case LightSourceKind::DirectionalLight:
            dto.value1 = {obj.transform.rotation, 0.f};
            break;

        case LightSourceKind::PointLight:
            dto.parameters.brightness = MIN_BRIGHTNESS * lightSource->radius * lightSource->radius;
            dto.value1 = {obj.transform.translation, 0.f};
            break;
        
        default:
            throw std::runtime_error("can't convert all LightSourceKinds to LightSourceDTO");
            break;
        }

        return dto;
    }
}