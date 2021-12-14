#pragma once

#include "../lve_game_object.hpp"
#include "../lve_device.hpp"
#include "../texture_manager.hpp"
#include "../material_manager.hpp"
#include "../model_manager.hpp"

// std
#include <vector>

namespace lve {

    struct Scenes { 
        static LveGameObject::Map loadTestScene1(LveDevice& device, LveGameObject& cameraObject, TextureManager& textureManager, MaterialManager& materialManager, ModelManager& modelManager);
        static LveGameObject::Map loadSceneLab1(LveDevice& device, LveGameObject& cameraObject, TextureManager& textureManager, MaterialManager& materialManager, ModelManager& modelManager);
        static LveGameObject::Map loadSceneLab3(LveDevice& device, LveGameObject& cameraObject, TextureManager& textureManager, MaterialManager& materialManager, ModelManager& modelManager);

    private:
        static void loadCoordinateSystem(LveDevice& device, LveGameObject::Map& gameObjects, TextureManager& textureManager);
    };
    
}