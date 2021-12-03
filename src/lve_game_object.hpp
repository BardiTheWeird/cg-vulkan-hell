#pragma once

#include "lve_model.hpp"
#include "components/light_source.hpp"
#include "components/transform_component.hpp"
#include "components/velocity_acceleration_component.hpp"
#include "components/circular_movement_component.hpp"
#include "components/material_component.hpp"
#include "components/repeat_movement_component.hpp"
#include "components/oscillator_component.hpp"

// std
#include <memory>
#include <unordered_map>
#include <string>
#include <optional>

namespace lve
{
    class OscillatorComponent;
    class LveGameObject {
    public:
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, LveGameObject>;

        LveGameObject(const LveGameObject &) = delete;
        LveGameObject &operator=(const LveGameObject &) = delete;
        LveGameObject(LveGameObject &&) = default;
        LveGameObject &operator=(LveGameObject &&) = default;


        static LveGameObject createGameObject() {
            static id_t currentId = 0;
            return LveGameObject{currentId++};
        }

        const id_t getId() { return id; }

        bool isVisible{true};
        std::shared_ptr<LveModel> model{};
        std::optional<std::string> textureKey = std::nullopt;
        std::optional<MaterialComponent> material = std::nullopt;

        // components
        // required
        TransformComponent transform{};

        // optional
        std::optional<LightSource> lightSource = std::nullopt;
        std::optional<VelocityAccelerationComponent> velocityAcceleration = std::nullopt;
        std::optional<CircularMovementComponent> circularMovement = std::nullopt;
        std::optional<RepeatMovementComponent> repeatMovement = std::nullopt;
        std::vector<std::shared_ptr<OscillatorComponent>> oscillators{};

    private:
        LveGameObject(id_t objId) : id{objId} {};
        const id_t id;
    };
} 
