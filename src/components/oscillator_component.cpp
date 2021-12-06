#include "oscillator_component.hpp"
#include "../utils/helpers.hpp"

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
                    oldAction(sampledValue, dt, gameObject, gameObjects, moveEvents);
                    action(sampledValue, dt, gameObject, gameObjects, moveEvents);
                };
                return *this;    
            }

            OscillatorComponent::Builder OscillatorComponent::Builder::AddAction(std::function<void (float, float, LveGameObject&, std::vector<MoveEvent>&)> action) {

                oscillator->actOnGameObject = [oldAction = move(oscillator->actOnGameObject), action = action]
                (float sampledValue, float dt, LveGameObject& gameObject, std::unordered_map<id_t, LveGameObject>& gameObjects, std::vector<MoveEvent>& moveEvents) {
                    oldAction(sampledValue, dt, gameObject, gameObjects, moveEvents);
                    action(sampledValue, dt, gameObject, moveEvents);
                };
                return *this;
            }

            OscillatorComponent::Builder OscillatorComponent::Builder::AddAction(std::function<void (float, float, LveGameObject&)> action) {

                oscillator->actOnGameObject = [oldAction = move(oscillator->actOnGameObject), action = action]
                (float sampledValue, float dt, LveGameObject& gameObject, std::unordered_map<id_t, LveGameObject>& gameObjects, std::vector<MoveEvent>& moveEvents) {
                    std::cout << "invoking action!" << std::endl;
                    oldAction(sampledValue, dt, gameObject, gameObjects, moveEvents);
                    action(sampledValue, dt, gameObject);
                };
                return *this;
            }

            OscillatorComponent::Builder OscillatorComponent::Builder::AddAction(std::function<void (float, LveGameObject&)> action) {

                oscillator->actOnGameObject = [oldAction = move(oscillator->actOnGameObject), action = action]
                    (float sampledValue, float dt, LveGameObject& gameObject, std::unordered_map<id_t, LveGameObject>& gameObjects, std::vector<MoveEvent>& moveEvents) 
                {
                    oldAction(sampledValue, dt, gameObject, gameObjects, moveEvents);
                    action(sampledValue, gameObject);
                };
                return *this;
            }

            OscillatorComponent::Builder OscillatorComponent::Builder::ShiftBy(glm::vec3 shift) {
                AddAction([shift=shift](float _, LveGameObject& obj) {
                    obj.transform.translation += shift;
                });

                return *this;
            }

            // Assumes that you set a position relative to {0, 0, 0} beforehand
            OscillatorComponent::Builder OscillatorComponent::Builder::MakeRelativeTo(id_t objId) {
                AddAction([objId=objId]
                (float _, float __, LveGameObject& gameObject, std::unordered_map<id_t, LveGameObject>& gameObjects, std::vector<MoveEvent>& ___){
                    gameObject.transform.translation += gameObjects.at(objId).transform.translation;
                });

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

    OscillatorComponent::Builder OscillatorComponent::GetEllipticMovement(float radius1, float radius2, float period, glm::vec3 rotation) {
        auto rotationMatrix = RotationHelpers::getRotationMatrix(rotation);
        return OscillatorComponent::Builder()
            .SetSamplingFunctionLinear(1.f, 0.f)
            .SetFrequency(1.f / period)
            .ScaleSamplingFunction(glm::two_pi<float>())
            .AddAction([radius1=radius1,radius2=radius2,rotationMatrix=rotationMatrix]
            (float sampledValue, float dt, LveGameObject& gameObject, std::unordered_map<id_t, LveGameObject>& gameObjects, std::vector<MoveEvent>& moveEvents){
                float angle = sampledValue;
                auto relativePosition = glm::vec3 {
                    std::cos(angle) * radius1,
                    0.f,
                    std::sin(angle) * radius2
                };

                gameObject.transform.translation = rotationMatrix * relativePosition;
            });
    }
}