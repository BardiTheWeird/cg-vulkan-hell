#include "oscillator_component.hpp"

namespace lve {

    void OscillatorComponent::advance(float frameTime, LveGameObject& gameObject, std::vector<MoveEvent>& moveEvents) {
        float range = (endValue - startValue);
        float speed = frequency * range;
        float distanceToMove = speed * frameTime;
        auto prevVal = curValue;
        curValue = glm::mod<float>(curValue + distanceToMove, range);

        float sampledValue = samplingFunction(curValue);

        actOnGameObject(sampledValue, std::abs(curValue - prevVal), gameObject, moveEvents);
    }
        

    OscillatorComponent::Builder OscillatorComponent::GetEllipticMovement(float xRadius, float zRadius) {
        auto getPosition = [xRadius=xRadius, zRadius=zRadius](float t) {
            return glm::vec2{
                xRadius * std::cos(t * glm::two_pi<float>()),
                zRadius * std::sin(t * glm::two_pi<float>())
            };
        };

        return OscillatorComponent::Builder()
            .SetSamplingFunctionLinear(1.f, 0.f)
            .AddAction([getPosition=getPosition](float sampledValue, float dt, LveGameObject& obj, std::vector<MoveEvent>& moveEvents) {
                MoveEvent event{};
                event.objectId = obj.getId();

                float tCur = sampledValue;
                float tPrev = (tCur - dt);
                tPrev = tPrev > 0.f
                    ? tPrev
                    : tPrev + 1.f;

                auto prevPosition = getPosition(tPrev);
                auto nextPosition = getPosition(tCur);

                auto shift = nextPosition - prevPosition;
                event.movement.x = shift.x;
                event.movement.z = shift.y;

                moveEvents.push_back(event);
            });
    }
}