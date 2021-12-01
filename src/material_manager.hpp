#pragma once

#include "lve_descriptors.hpp"
#include "lve_buffer.hpp"
#include "components/material_component.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <unordered_map>

namespace lve {

    class MaterialManager {
    public:
        using id_t = unsigned int;

        MaterialManager(const MaterialManager&) = delete;
        MaterialManager &operator=(const MaterialManager&) = delete;

        MaterialManager(LveDevice& device);
        ~MaterialManager();

        const VkDescriptorSetLayout getDescriptorSetLayout() const { return (*materialDescriptorSetLayout).getDescriptorSetLayout(); }
        const VkDescriptorSet getDescriptorSet(id_t id) const { return perMaterialAllocations.at(id).descriptorSet; }

        id_t allocateMaterial();
        void updateMaterial(MaterialComponent& materialComponent);

    private:
        struct MaterialUbo {
            glm::vec4 albedoAndRoughness{}; // albedo at .xyz; roughness at .w
            glm::vec4 baseReflectanceAndMetallicCoefficient{}; // baseReflectance at .xyz; metallicCoefficient at .w
            glm::vec4 emissivityMesh{}; // .w is empty

            static MaterialUbo fromComponent(const MaterialComponent& component) {
                return MaterialUbo {
                    {component.albedoMesh, component.roughness},
                    {component.baseReflectance, component.metallicCoefficient},
                    {component.emissivityMesh}
                };
            }
        };
        
        struct PerMaterialAllocations {
            std::unique_ptr<LveBuffer> buffer;
            VkDescriptorSet descriptorSet;
        };
        using Map = std::unordered_map<id_t, PerMaterialAllocations>;

        void createDescriptorSetLayout();
        void createDescriptorPool();

        void destroyMaterialAllocations();

        Map perMaterialAllocations{};

        LveDevice& lveDevice;

        std::unique_ptr<LveDescriptorPool> materialDescriptorPool;
        std::unique_ptr<LveDescriptorSetLayout> materialDescriptorSetLayout;
    };
}