#include "texture_manager.hpp"

// std
#include <stdexcept>

namespace lve {

    TextureManager::TextureManager(LveDevice& device) : lveDevice{device} {
        createDescriptorSetLayout();
        createDescriptorPool();
    }

    TextureManager::~TextureManager() {
        // vkDestroyDescriptorPool(lveDevice.device(), textureDescriptorPool, nullptr);
        // vkDestroyDescriptorSetLayout(lveDevice.device(), getTextureDescriptorSetLayout(), nullptr);

        destroyTextureAllocations();
    }

    void TextureManager::createDescriptorSetLayout() {
        auto descriptorSetLayout = LveDescriptorSetLayout::Builder(lveDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .build();

        textureDescriptorSetLayout = std::move(descriptorSetLayout);
    }

    void TextureManager::createDescriptorPool() {
        auto poolPtr = LveDescriptorPool::Builder(lveDevice)
            .setMaxSets(100)
            .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100)
            .build();

        textureDescriptorPool = std::move(poolPtr);
    }

    VkDescriptorSet TextureManager::createDescriptorSet(VkImageView& imageView, VkSampler& imageSampler) {
        VkDescriptorSet descriptorSet;
        (*textureDescriptorPool).allocateDescriptorSet(
            getTextureDescriptorSetLayout(), descriptorSet);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = imageView;
        imageInfo.sampler = imageSampler;

        LveDescriptorWriter(*textureDescriptorSetLayout, *textureDescriptorPool)
            .writeImage(0, &imageInfo)
            .build(descriptorSet);

        return descriptorSet;
    }

    void TextureManager::addTexture(std::string filepath, std::string key) {
        PerTextureAllocations allocations{};

        lveDevice.createTextureImage(filepath, allocations.image, allocations.imageMemory);
        allocations.imageView = lveDevice.createTextureImageView(allocations.image);
        allocations.imageSampler = lveDevice.createTextureSampler(allocations.image, allocations.imageView);
        VkDescriptorSet descriptorSet = createDescriptorSet(allocations.imageView, allocations.imageSampler);

        textureAllocations.push_back(std::move(allocations));
        textureDescriptorSets.emplace(key, descriptorSet);
    }

    void TextureManager::destroyTextureAllocations() {
        for (auto& allocations: textureAllocations) {
            vkDestroySampler(lveDevice.device(), allocations.imageSampler, nullptr);
            vkDestroyImageView(lveDevice.device(), allocations.imageView, nullptr);
            vkDestroyImage(lveDevice.device(), allocations.image, nullptr);
            vkFreeMemory(lveDevice.device(), allocations.imageMemory, nullptr);
        }
    }
}