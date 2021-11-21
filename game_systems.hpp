#pragma once

#include "lve_frame_info.hpp"

// std
#include <vector>

namespace lve {

    struct GameSystems {
        static void executeAll(GameSystemInfo& gameSystemInfo);
        static void move(GameSystemInfo& gameSystemInfo);
        static void moveCircle(GameSystemInfo& gameSystemInfo);
    };
}