#pragma once

#include "lve_frame_info.hpp"

// std
#include <vector>

namespace lve {

    struct GameSystems {
        static void executeAll(FrameInfo& frameInfo);
        static void move(FrameInfo& frameInfo);
        static void moveCircle(FrameInfo& frameInfo);
    };
}