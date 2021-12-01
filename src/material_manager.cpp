#include "material_manager.hpp"

namespace lve {
    MaterialManager::MaterialManager(LveDevice& device) : lveDevice{device} {
        createDescriptorSetLayout();
        createDescriptorPool();
    }

    MaterialManager::~MaterialManager() {
        destroyMaterialAllocations();
    }


    void MaterialManager::createDescriptorSetLayout() {
        auto layout = LveDescriptorSetLayout::Builder(lveDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .build();

        materialDescriptorSetLayout = std::move(layout);
    }

    void MaterialManager::createDescriptorPool() {
        auto pool = LveDescriptorPool::Builder(lveDevice)
            .setMaxSets(100)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100)
            .build();

        materialDescriptorPool = std::move(pool);
    }


    void MaterialManager::destroyMaterialAllocations() {
        // doesn't look like i need to do anything, lol
    }


    id_t MaterialManager::allocateMaterial() {
        static id_t curMaxId = 0;

        auto allocations = PerMaterialAllocations{};
        allocations.buffer = std::make_unique<LveBuffer>(
            lveDevice,
            sizeof(MaterialUbo),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        allocations.buffer->map();

        materialDescriptorPool->allocateDescriptorSet(getDescriptorSetLayout(), allocations.descriptorSet);

        auto descriptorInfo = allocations.buffer->descriptorInfo();
        LveDescriptorWriter(*materialDescriptorSetLayout, *materialDescriptorPool)
            .writeBuffer(0, &descriptorInfo)
            .build(allocations.descriptorSet);

        perMaterialAllocations.emplace(curMaxId, std::move(allocations));
        
        return curMaxId++;
    }

    void MaterialManager::updateMaterial(MaterialComponent& component) {
        auto& allocations = perMaterialAllocations.at(component.material_id);

        auto ubo = MaterialUbo::fromComponent(component);
        allocations.buffer->writeToBuffer((void*)&ubo);
    }
}