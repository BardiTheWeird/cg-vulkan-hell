#pragma once

#include "lve_device.hpp"
#include "lve_descriptors.hpp"

// std
#include <string>
#include <unordered_map>

namespace lve {

    class TextureManager
    {
    public:
        TextureManager(const TextureManager&) = delete;
        TextureManager &operator=(const TextureManager&) = delete;

        TextureManager(LveDevice& device);
        ~TextureManager();

        void addTexture(std::string filepath, std::string key);

        const VkDescriptorSetLayout getTextureDescriptorSetLayout() const { return (*textureDescriptorSetLayout).getDescriptorSetLayout(); }
        const bool getTextureDescriptorSet(std::string key, VkDescriptorSet& descriptorSet) const {
            try {
                descriptorSet = textureDescriptorSets.at(key);
                return true;
            }
            catch (const char* message) {
                return false;
            }
        }

    private:
        using Map = std::unordered_map<std::string, VkDescriptorSet>;
        struct PerTextureAllocations {
            VkImage image;
            VkDeviceMemory imageMemory;
            VkImageView imageView;
            VkSampler imageSampler;
        };

        void createDescriptorSetLayout();
        void createDescriptorPool();

        void destroyTextureAllocations();

        VkDescriptorSet createDescriptorSet(VkImageView& imageView, VkSampler& imageSampler);

        std::vector<PerTextureAllocations> textureAllocations{};
        Map textureDescriptorSets{};

        LveDevice& lveDevice;

        std::unique_ptr<LveDescriptorPool> textureDescriptorPool;
        std::unique_ptr<LveDescriptorSetLayout> textureDescriptorSetLayout;
    };
}