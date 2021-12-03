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
        float startValue{0.f};
        float endValue{1.f};
        float frequency{1.f};

        std::function<float (float)> samplingFunction{[](float x){return std::sin(x * 2 * glm::pi<float>());}};
        std::function<void (float, float, LveGameObject&, std::vector<MoveEvent>&)> actOnGameObject{
            [](float sampledValue, float dt, LveGameObject& gameObject, std::vector<MoveEvent>& moveEvents) {}
        };

        float curValue{0.f};

        void advance(float frameTime, LveGameObject& gameObject, std::vector<MoveEvent>& moveEvents);

        class Builder {
        private:
            OscillatorComponent* oscillator = new OscillatorComponent();

        public:
            Builder SetSamplingFunction(std::function<float (float)> function) {
                oscillator->samplingFunction = function;
                return *this;
            }

            Builder PipeSamplingFunction(std::function<float (float)> nextFunc) {
                oscillator->samplingFunction = [oldFunction = move(oscillator->samplingFunction), nextFunc = nextFunc]
                (float t) {
                    return nextFunc(oldFunction(t));
                };

                return *this;
            }

            Builder ScaleSamplingFunction(float coeff) {
                return PipeSamplingFunction([coeff=coeff](float t){ return t * coeff; });
            }

            Builder ShiftSamplingFunction(float amount) {
                return PipeSamplingFunction([amount=amount](float t){ return t + amount; });
            }

            Builder SetSamplingFunctionLinear(float k, float b) {
                return SetSamplingFunction([k=k, b=b](float x){ return k*x + b; });
            }

            Builder SetSamplingFunctionSquare(float magnitude = 1.f) {
                return SetSamplingFunction([magnitude=magnitude](float t) {
                    if (t <.5f)
                        return magnitude;
                    return -magnitude;
                });
            }

            Builder SetSamplingFunctionSin(float shiftVertical, float scale) {
                return SetSamplingFunction([shiftVertical = shiftVertical, scale = scale](float x){ 
                    return scale * (std::sin(x * 2 * glm::pi<float>()) + shiftVertical);
                });
            }

            Builder AddAction(std::function<void (float, float, LveGameObject&, std::vector<MoveEvent>&)> action) {

                oscillator->actOnGameObject = [oldAction = move(oscillator->actOnGameObject), action = action]
                (float sampledValue, float dt, LveGameObject& gameObject, std::vector<MoveEvent>& moveEvents) {
                    action(sampledValue, dt, gameObject, moveEvents);
                    oldAction(sampledValue, dt, gameObject, moveEvents);
                };
                return *this;
            }

            Builder AddAction(std::function<void (float, float, LveGameObject&)> action) {

                oscillator->actOnGameObject = [oldAction = move(oscillator->actOnGameObject), action = action]
                (float sampledValue, float dt, LveGameObject& gameObject, std::vector<MoveEvent>& moveEvents) {
                    std::cout << "invoking action!" << std::endl;
                    action(sampledValue, dt, gameObject);
                    oldAction(sampledValue, dt, gameObject, moveEvents);
                };
                return *this;
            }

            Builder AddAction(std::function<void (float, LveGameObject&)> action) {

                oscillator->actOnGameObject = [oldAction = move(oscillator->actOnGameObject), action = action]
                    (float sampledValue, float dt, LveGameObject& gameObject, std::vector<MoveEvent>& moveEvents) 
                {
                    action(sampledValue, gameObject);
                    oldAction(sampledValue, dt, gameObject, moveEvents);
                };
                return *this;
            }

            Builder SetBounds(float min, float max) {
                oscillator->startValue = min;
                oscillator->endValue = max;
                return *this;
            }

            Builder SetFrequency(float frequency) {
                oscillator->frequency = frequency;
                return *this;
            }

            Builder SetPhase(float phase) {
                float phaseMapped = glm::mod<float>(phase, 180.f) / 180.f;
                oscillator->curValue = phaseMapped > 0.f
                    ? phaseMapped
                    : 1 - phaseMapped;
                return *this;
            }

            std::shared_ptr<OscillatorComponent> Build() {
                return std::make_shared<OscillatorComponent>(*oscillator);
            }
        };

    static Builder GetEllipticMovement(float xRadius, float zRadius);
    };
}