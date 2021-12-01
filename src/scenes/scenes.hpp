#pragma once

#include "../lve_game_object.hpp"
#include "../lve_device.hpp"
#include "../texture_manager.hpp"
#include "../material_manager.hpp"

// std
#include <vector>

namespace lve {

    struct Scenes {
        static LveGameObject::Map loadTestScene1(LveDevice& device, TextureManager& textureManager, MaterialManager& materialManager);

    private:
        static void loadCoordinateSystem(LveDevice& device, LveGameObject::Map& gameObjects, TextureManager& textureManager);
    };
    
}