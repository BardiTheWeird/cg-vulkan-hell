#pragma once

#include "lve_model.hpp"
#include "components/light_source.hpp"
#include "components/transform_component.hpp"
#include "components/velocity_acceleration_component.hpp"
#include "components/circular_movement_component.hpp"

// std
#include <memory>

namespace lve
{
    class LveGameObject {
    public:
        LveGameObject(const LveGameObject &) = delete;
        LveGameObject &operator=(const LveGameObject &) = delete;
        LveGameObject(LveGameObject &&) = default;
        LveGameObject &operator=(LveGameObject &&) = default;


        using id_t = unsigned int;
        static LveGameObject createGameObject() {
            static id_t currentId = 0;
            return LveGameObject{currentId++};
        }

        const id_t getId() { return id; }

        bool isVisible{true};
        std::shared_ptr<LveModel> model{};

        // components
        // required
        TransformComponent transform{};

        // optional
        std::shared_ptr<LightSource> lightSource = nullptr;
        std::shared_ptr<VelocityAccelerationComponent> velocityAcceleration = nullptr;
        std::shared_ptr<CircularMovementComponent> circularMovement = nullptr;

    private:
        LveGameObject(id_t objId) : id{objId} {};
        const id_t id;
    };
} 
