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
            [](float sampledValue, float frameTime, LveGameObject& gameObject, std::vector<MoveEvent>& moveEvents) {}
        };

        float curValue{0.f};

        void advance(float frameTime, LveGameObject& gameObject, std::vector<MoveEvent>& moveEvents);

        class Builder {
        private:
            OscillatorComponent* oscillator{};

        public:
            Builder SetSamplingFunction(std::function<float (float)> function) {
                oscillator->samplingFunction = function;
                return *this;
            }

            // Builder AddAction(std::function<void (float, float, LveGameObject&, std::vector<MoveEvent>&)> action) {
            //     oscillator->actOnGameObject = [&](float sampledValue, float frameTime, LveGameObject& gameObject, std::vector<MoveEvent>& moveEvents) {
            //         std::cout << "invoking action!" << std::endl;
            //         action(sampledValue, frameTime, gameObject, moveEvents);
            //         oscillator->actOnGameObject(sampledValue, frameTime, gameObject, moveEvents);
            //     };
            //     return *this;
            // }

            Builder AddAction(std::function<void (float, LveGameObject&)> action) {
                oscillator->actOnGameObject = [oldAction = oscillator->actOnGameObject, action = action]
                    (float sampledValue, float frameTime, LveGameObject& gameObject, std::vector<MoveEvent>& moveEvents) 
                {
                    std::cout << "invoking action!" << std::endl;
                    action(sampledValue, gameObject);
                    oldAction(sampledValue, frameTime, gameObject, moveEvents);
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

            OscillatorComponent* Build() {
                return oscillator;
            }
        };
    };
}