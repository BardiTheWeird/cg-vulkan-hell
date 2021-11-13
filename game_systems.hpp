#pragma once

#include "lve_game_object.hpp"

// std
#include <vector>

namespace lve {

    struct GameSystems {
        static void executeAll(std::vector<LveGameObject>& gameObjects, float frameTime);
        static void move(std::vector<LveGameObject>& gameObjects, float frameTime);
        static void moveCircle(std::vector<LveGameObject>& gameObjects, float frameTime);
    };
}