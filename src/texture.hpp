#include "lve_descriptors.hpp"
#include "lve_buffer.hpp"
#include "lve_descriptors.hpp"

//std
#include <unordered_map>
#include <memory>

namespace lve {

    class Texture
    {
    public:
        // using id_t = unsigned int;
        // using Map = std::unordered_map<id_t, Texture>;

        Texture(const Texture &) = delete;
        Texture &operator=(const Texture &) = delete;
        Texture(Texture &&) = default;
        Texture &operator=(Texture &&) = default;

        ~Texture();

        static Texture createFromFile(
            LveDevice& lveDevice, 
            const std::string& filepath);

        static VkDescriptorSetLayout getDescriptorSetLayout(LveDevice& lveDevice);

        // const id_t getId() const { return id; }
        const VkImageView getImageView() const { return imageView; };
        const VkSampler getSampler() const { return imageSampler; };
        const VkDescriptorSet getDescriptorSet() const { return textureDescriptorSet; };

    private:
        Texture(LveDevice& device, VkImage _image, VkDeviceMemory _imageMemory, VkImageView imageView, VkSampler imageSampler, VkDescriptorSet _descriptorSet);

        void constructImageView();
        void constructImageSampler();
        void constructDescriptorSet(LveDescriptorPool& textureDescriptorPool, LveDescriptorSetLayout& textureDescriptorSetLayout);

        // const id_t id;

        LveDevice& lveDevice;

        VkImage image;
        VkDeviceMemory imageMemory;
        VkImageView imageView;
        VkSampler imageSampler;

        VkDescriptorSet textureDescriptorSet;
    };
}