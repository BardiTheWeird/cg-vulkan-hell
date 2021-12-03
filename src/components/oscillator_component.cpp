#include "oscillator_component.hpp"

namespace lve {

    void OscillatorComponent::advance(float frameTime, LveGameObject& gameObject, std::vector<MoveEvent>& moveEvents) {
        float range = (endValue - startValue);
        float speed = frequency * range;
        float distanceToMove = speed * frameTime;
        curValue = glm::mod<float>(curValue + distanceToMove, range);

        float sampledValue = samplingFunction(curValue);

        actOnGameObject(sampledValue, frameTime, gameObject, moveEvents);
    }
        
}