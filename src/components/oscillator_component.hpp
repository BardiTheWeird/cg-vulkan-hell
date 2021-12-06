#pragma once

#include "../lve_game_object.hpp"
#include "../move_event.hpp"

// std
#include <functional>
#include <optional>
#include <math.h>
#include <iostream>

namespace lve {

    struct OscillatorComponent {
        using id_t = unsigned int;
        float startValue{0.f};
        float endValue{1.f};
        float frequency{1.f};

        std::function<float (float)> samplingFunction{[](float x){return std::sin(x * 2 * glm::pi<float>());}};
        std::function<void (float, float, LveGameObject&, std::unordered_map<id_t, LveGameObject>& gameObjects, std::vector<MoveEvent>&)> actOnGameObject{
            [](float sampledValue, float dt, LveGameObject& gameObject, std::unordered_map<id_t, LveGameObject>& gameObjects, std::vector<MoveEvent>& moveEvents) {}
        };

        float curValue{0.f};

        void advance(float frameTime, LveGameObject& gameObject, std::unordered_map<id_t, LveGameObject>& gameObjects, std::vector<MoveEvent>& moveEvents);

        class Builder {
        private:
            OscillatorComponent* oscillator = new OscillatorComponent();

        public:
            Builder SetSamplingFunction(std::function<float (float)> function);

            Builder PipeSamplingFunction(std::function<float (float)> nextFunc);

            Builder ScaleSamplingFunction(float coeff);

            Builder ShiftSamplingFunction(float amount);

            Builder SetSamplingFunctionLinear(float k, float b);

            Builder SetSamplingFunctionSquare(float magnitude = 1.f);

            Builder SetSamplingFunctionSin(float shiftVertical, float scale);

            Builder AddAction(std::function<void (float, float, LveGameObject&, std::vector<MoveEvent>&)> action);

            Builder AddAction(std::function<void (float, float, LveGameObject&)> action);

            Builder AddAction(std::function<void (float, LveGameObject&)> action);

            Builder SetBounds(float min, float max);

            Builder SetFrequency(float frequency);

            Builder SetPhase(float phase);
            
            std::shared_ptr<OscillatorComponent> Build();
        };

    static Builder GetLinearMovement(glm::vec3 maxDisplacement, float period);

    static Builder GetEllipticMovement(float xRadius, float zRadius);

    // static Builder GetCircularMovementAroundAnObject(LveGameObject::std::unordered_map<id_t, LveGameObject>& gameObjects, LveGameObject::id_t objId, float radius);
    };
}