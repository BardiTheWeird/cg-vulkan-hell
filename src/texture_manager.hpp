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
        using Map = std::unordered_map<std::string, VkDescriptorSet>;

        TextureManager(const TextureManager&) = delete;
        TextureManager &operator=(const TextureManager&) = delete;

        TextureManager(LveDevice& device);
        ~TextureManager();

        void addTexture(std::string filepath, std::string key);

        const VkDescriptorSetLayout getTextureDescriptorSetLayout() const { return textureDescriptorSetLayout; }
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

        VkDescriptorPool textureDescriptorPool;
        VkDescriptorSetLayout textureDescriptorSetLayout;
    };
}