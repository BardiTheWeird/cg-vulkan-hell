#pragma once

#include "low-level/lve_window.hpp"
#include "lve_game_object.hpp"
#include "lve_camera.hpp"

namespace lve {

    class KeyboardMovementController {
    public:
        struct KeyMappings {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_E;
            int moveDown = GLFW_KEY_Q;
            int lookLeft = GLFW_KEY_LEFT;
            int lookRight = GLFW_KEY_RIGHT;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;
            int switchProjections = GLFW_KEY_P;
        };

        void moveInPlaneXZ(GLFWwindow* window, float dt, LveGameObject& gameObject);
        bool shouldSwitchProjection(GLFWwindow* window);

        KeyMappings keys{};
        float moveSpeed{3.f};
        float currentMoveSpeed{0.f};
        float moveAcceleration{.3f};

        float turnSpeed{1.5f};
    };
}