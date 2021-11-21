#pragma once

#include "../lve_game_object.hpp"
#include "../lve_device.hpp"

// std
#include <vector>

namespace lve {

    struct Scenes {
        static LveGameObject::Map loadTestScene1(LveDevice& device);

    private:
        static void loadCoordinateSystem(LveDevice& device, LveGameObject::Map& gameObjects);
    };
    
}