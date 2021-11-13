#pragma once

#include "../lve_game_object.hpp"
#include "../lve_device.hpp"

// std
#include <vector>

namespace lve {

    struct Scenes {
        static std::vector<LveGameObject> loadTestScene1(LveDevice& device);
    };
    
}