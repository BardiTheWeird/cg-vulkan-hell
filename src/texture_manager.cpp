#include "texture_manager.hpp"

// std
#include <stdexcept>

namespace lve {

    TextureManager::TextureManager(LveDevice& device) : lveDevice{device} {
        createDescriptorSetLayout();
        createDescriptorPool();
    }

    TextureManager::~TextureManager() {
        vkDestroyDescriptorPool(lveDevice.device(), textureDescriptorPool, nullptr);
        vkDestroyDescriptorSetLayout(lveDevice.device(), textureDescriptorSetLayout, nullptr);

        destroyTextureAllocations();
    }

    void TextureManager::createDescriptorSetLayout() {
        VkDescriptorSetLayoutBinding texDescSetLayoutBinding{};
        texDescSetLayoutBinding.binding = 0;
        texDescSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        texDescSetLayoutBinding.descriptorCount = 1;
        texDescSetLayoutBinding.pImmutableSamplers = nullptr;
        texDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &texDescSetLayoutBinding;

        if (vkCreateDescriptorSetLayout(lveDevice.device(), &layoutInfo, nullptr, &textureDescriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void TextureManager::createDescriptorPool() {
        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSize.descriptorCount = 1;
        
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = 1000;

        if (vkCreateDescriptorPool(lveDevice.device(), &poolInfo, nullptr, &textureDescriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    VkDescriptorSet TextureManager::createDescriptorSet(VkImageView& imageView, VkSampler& imageSampler) {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = textureDescriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &textureDescriptorSetLayout;

        VkDescriptorSet descriptorSet;
        if (vkAllocateDescriptorSets(lveDevice.device(), &allocInfo, &descriptorSet) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = imageView;
        imageInfo.sampler = imageSampler;

        VkWriteDescriptorSet imageDescriptorWrite;
        imageDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        imageDescriptorWrite.dstSet = descriptorSet;
        imageDescriptorWrite.dstBinding = 0;
        imageDescriptorWrite.dstArrayElement = 0;
        imageDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        imageDescriptorWrite.descriptorCount = 1;
        imageDescriptorWrite.pImageInfo = &imageInfo;
        imageDescriptorWrite.pNext = nullptr;

        vkUpdateDescriptorSets(lveDevice.device(), 1, &imageDescriptorWrite, 0, nullptr);

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