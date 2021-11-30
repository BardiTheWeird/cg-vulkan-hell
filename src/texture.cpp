#include "texture.hpp"

// std
#include <iostream>
#include <stdexcept>
#include <cstring>

// libs
#include <stb_image.h>

namespace lve {

    Texture::Texture(LveDevice& device, VkImage _image, VkDeviceMemory _imageMemory, VkImageView _imageView, VkSampler _imageSampler, VkDescriptorSet _descriptorSet) : lveDevice{device} {
        if (_image == VK_NULL_HANDLE) {
            throw std::runtime_error("image is VK_NULL_HANDLE");
        }

        if (_imageMemory == VK_NULL_HANDLE) {
            throw std::runtime_error("imageMemory is VK_NULL_HANDLE");
        }

        image = _image;
        imageMemory = _imageMemory;
        imageView = _imageView;
        imageSampler = _imageSampler;
        textureDescriptorSet = _descriptorSet;
    }

    Texture::~Texture() {
        // vkDestroySampler(lveDevice.device(), imageSampler, nullptr);
        // vkDestroyImageView(lveDevice.device(), imageView, nullptr);
        // vkDestroyImage(lveDevice.device(), image, nullptr);
        // vkFreeMemory(lveDevice.device(), imageMemory, nullptr);
    }

    VkDescriptorPool createDescriptorPool(LveDevice& lveDevice) {
        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSize.descriptorCount = 1;
        
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = 1;

        VkDescriptorPool descriptorPool;
        if (vkCreateDescriptorPool(lveDevice.device(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }

        return descriptorPool;
    }

    VkDescriptorSet createDescriptorSet(LveDevice& lveDevice, VkImageView textureImageView, VkSampler textureSampler) {
        auto descriptorPool = createDescriptorPool(lveDevice);

        VkDescriptorSetLayout textureDescriptorSetLayout = Texture::getDescriptorSetLayout(lveDevice);

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &textureDescriptorSetLayout;

        VkDescriptorSet descriptorSet;
        if (vkAllocateDescriptorSets(lveDevice.device(), &allocInfo, &descriptorSet) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }



        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = textureImageView;
        imageInfo.sampler = textureSampler;

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

    Texture Texture::createFromFile(
        LveDevice& lveDevice, 
        const std::string& filepath) {

        VkImage image;
        VkDeviceMemory imageMemory;
        lveDevice.createTextureImage(filepath, image, imageMemory);
        VkImageView imageView = lveDevice.createTextureImageView(image);
        VkSampler imageSampler = lveDevice.createTextureSampler(image, imageView);
        VkDescriptorSet descriptorSet = createDescriptorSet(lveDevice, imageView, imageSampler);

        Texture texture{lveDevice, image, imageMemory, imageView, imageSampler, descriptorSet};

        return texture;
    }

    void Texture::constructDescriptorSet(LveDescriptorPool& textureDescriptorPool, LveDescriptorSetLayout& textureDescriptorSetLayout) {
        VkDescriptorImageInfo imageBufferInfo;
        imageBufferInfo.sampler = imageSampler;
        imageBufferInfo.imageView = imageView;
        imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        LveDescriptorWriter(textureDescriptorSetLayout, textureDescriptorPool)
            .writeImage(0, &imageBufferInfo)
            .build(textureDescriptorSet);
    }

    VkDescriptorSetLayout Texture::getDescriptorSetLayout(LveDevice& lveDevice) {
        static VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;

        if (descriptorSetLayout != VK_NULL_HANDLE) {
            return descriptorSetLayout;
        }

        descriptorSetLayout = LveDescriptorSetLayout::Builder(lveDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .build()
            .get()->getDescriptorSetLayout();

        return descriptorSetLayout;
    }
}