#include "oscillator_component.hpp"

namespace lve {

    void OscillatorComponent::advance(float frameTime, LveGameObject& gameObject, std::unordered_map<id_t, LveGameObject>& gameObjects, std::vector<MoveEvent>& moveEvents) {
        float range = (endValue - startValue);
        float speed = frequency * range;
        float distanceToMove = speed * frameTime;
        auto prevVal = curValue;
        curValue = glm::mod<float>(curValue + distanceToMove, range);

        float sampledValue = samplingFunction(curValue);

        actOnGameObject(sampledValue, std::abs(curValue - prevVal), gameObject, gameObjects, moveEvents);
    }
        
            OscillatorComponent::Builder OscillatorComponent::Builder::SetSamplingFunction(std::function<float (float)> function) {
                oscillator->samplingFunction = function;
                return *this;
            }

            OscillatorComponent::Builder OscillatorComponent::Builder::PipeSamplingFunction(std::function<float (float)> nextFunc) {
                oscillator->samplingFunction = [oldFunction = move(oscillator->samplingFunction), nextFunc = nextFunc]
                (float t) {
                    return nextFunc(oldFunction(t));
                };

                return *this;
            }

            OscillatorComponent::Builder OscillatorComponent::Builder::ScaleSamplingFunction(float coeff) {
                return PipeSamplingFunction([coeff=coeff](float t){ return t * coeff; });
            }

            OscillatorComponent::Builder OscillatorComponent::Builder::ShiftSamplingFunction(float amount) {
                return PipeSamplingFunction([amount=amount](float t){ return t + amount; });
            }

            OscillatorComponent::Builder OscillatorComponent::Builder::SetSamplingFunctionLinear(float k, float b) {
                return SetSamplingFunction([k=k, b=b](float x){ return k*x + b; });
            }

            OscillatorComponent::Builder OscillatorComponent::Builder::SetSamplingFunctionSquare(float magnitude) {
                return SetSamplingFunction([magnitude=magnitude](float t) {
                    if (t <.5f)
                        return magnitude;
                    return -magnitude;
                });
            }

            OscillatorComponent::Builder OscillatorComponent::Builder::SetSamplingFunctionSin(float shiftVertical, float scale) {
                return SetSamplingFunction([shiftVertical = shiftVertical, scale = scale](float x){ 
                    return scale * (std::sin(x * 2 * glm::pi<float>()) + shiftVertical);
                });
            }

            OscillatorComponent::Builder OscillatorComponent::Builder::AddAction(
                std::function<void (float, float, LveGameObject&, std::unordered_map<id_t, LveGameObject>&, std::vector<MoveEvent>&)> action) {

                oscillator->actOnGameObject = [oldAction = move(oscillator->actOnGameObject), action = action]
                (float sampledValue, float dt, LveGameObject& gameObject, std::unordered_map<id_t, LveGameObject>& gameObjects, std::vector<MoveEvent>& moveEvents) {
                    action(sampledValue, dt, gameObject, gameObjects, moveEvents);
                    oldAction(sampledValue, dt, gameObject, gameObjects, moveEvents);
                };
                return *this;    
            }

            OscillatorComponent::Builder OscillatorComponent::Builder::AddAction(std::function<void (float, float, LveGameObject&, std::vector<MoveEvent>&)> action) {

                oscillator->actOnGameObject = [oldAction = move(oscillator->actOnGameObject), action = action]
                (float sampledValue, float dt, LveGameObject& gameObject, std::unordered_map<id_t, LveGameObject>& gameObjects, std::vector<MoveEvent>& moveEvents) {
                    action(sampledValue, dt, gameObject, moveEvents);
                    oldAction(sampledValue, dt, gameObject, gameObjects, moveEvents);
                };
                return *this;
            }

            OscillatorComponent::Builder OscillatorComponent::Builder::AddAction(std::function<void (float, float, LveGameObject&)> action) {

                oscillator->actOnGameObject = [oldAction = move(oscillator->actOnGameObject), action = action]
                (float sampledValue, float dt, LveGameObject& gameObject, std::unordered_map<id_t, LveGameObject>& gameObjects, std::vector<MoveEvent>& moveEvents) {
                    std::cout << "invoking action!" << std::endl;
                    action(sampledValue, dt, gameObject);
                    oldAction(sampledValue, dt, gameObject, gameObjects, moveEvents);
                };
                return *this;
            }

            OscillatorComponent::Builder OscillatorComponent::Builder::AddAction(std::function<void (float, LveGameObject&)> action) {

                oscillator->actOnGameObject = [oldAction = move(oscillator->actOnGameObject), action = action]
                    (float sampledValue, float dt, LveGameObject& gameObject, std::unordered_map<id_t, LveGameObject>& gameObjects, std::vector<MoveEvent>& moveEvents) 
                {
                    action(sampledValue, gameObject);
                    oldAction(sampledValue, dt, gameObject, gameObjects, moveEvents);
                };
                return *this;
            }

            OscillatorComponent::Builder OscillatorComponent::Builder::SetBounds(float min, float max) {
                oscillator->startValue = min;
                oscillator->endValue = max;
                return *this;
            }

            OscillatorComponent::Builder OscillatorComponent::Builder::SetFrequency(float frequency) {
                oscillator->frequency = frequency;
                return *this;
            }

            OscillatorComponent::Builder OscillatorComponent::Builder::SetPhase(float phase) {
                float phaseMapped = glm::mod<float>(phase, 180.f) / 180.f;
                oscillator->curValue = phaseMapped > 0.f
                    ? phaseMapped
                    : 1 - phaseMapped;
                return *this;
            }

            std::shared_ptr<OscillatorComponent> OscillatorComponent::Builder::Build() {
                return std::make_shared<OscillatorComponent>(*oscillator);
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

    // OscillatorComponent::Builder OscillatorComponent::GetCircularMovementAroundAnObject(LveGameObject::Map& gameObjects, LveGameObject::id_t objId, float radius) {
    //     return Builder()
    //         .SetSamplingFunctionLinear(1.f, 0.f)
    //         .ScaleSamplingFunction(glm::two_pi<float>())
    //         .AddAction([objs=gameObjects, objId=objId, radius=radius](float sampledValue, LveGameObject& obj) {
    //             glm::vec3 relativePosition{
    //                 std::cos(sampledValue),
    //                 0.f,
    //                 std::sin(sampledValue)
    //             };
                
    //             relativePosition *= radius;

    //             obj.transform.translation = objs.at(objId).transform.translation + relativePosition;
    //         });
    // }
}