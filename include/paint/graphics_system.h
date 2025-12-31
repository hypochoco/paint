////
////  graphics_system.h
////  paint
////
////  Created by Daniel Cho on 12/23/25.
////
//
//#pragma once
//
//#include <vulkan/vulkan.h>
//#include <qlogging.h>
//
//#include <vector>
//#include <fstream>
//
//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#define GLM_ENABLE_EXPERIMENTAL
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/hash.hpp>
//
////#define STB_IMAGE_IMPLEMENTATION // todo: where is the duplicate ??
//#include <stb_image.h>
//
//struct Vertex {
//    glm::vec3 pos;
//    glm::vec3 color;
//    glm::vec2 texCoord;
//
//    static VkVertexInputBindingDescription getBindingDescription() {
//        VkVertexInputBindingDescription bindingDescription{};
//        bindingDescription.binding = 0;
//        bindingDescription.stride = sizeof(Vertex);
//        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
//
//        return bindingDescription;
//    }
//
//    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
//        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
//
//        attributeDescriptions[0].binding = 0;
//        attributeDescriptions[0].location = 0;
//        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
//        attributeDescriptions[0].offset = offsetof(Vertex, pos);
//
//        attributeDescriptions[1].binding = 0;
//        attributeDescriptions[1].location = 1;
//        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
//        attributeDescriptions[1].offset = offsetof(Vertex, color);
//
//        attributeDescriptions[2].binding = 0;
//        attributeDescriptions[2].location = 2;
//        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
//        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);
//
//        return attributeDescriptions;
//    }
//
//    bool operator==(const Vertex& other) const {
//        return pos == other.pos && color == other.color && texCoord == other.texCoord;
//    }
//};
//
//namespace std {
//    template<> struct hash<Vertex> {
//        size_t operator()(Vertex const& vertex) const {
//            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
//        }
//    };
//}
//
//
//struct GlobalUBO {
//    alignas(16) glm::mat4 view;
//    alignas(16) glm::mat4 proj;
//    // todo: lights
//};
//
//struct InstanceSSBO {
//    alignas(16) glm::mat4 model;
//};
//
//class GraphicsSystem {
//    
//    // todo: create and manage textures
//    // todo: create and manage pipelines
//    // todo: cleanup functions
//  
//public:
//    
//    // constants
//    
//    int MAX_FRAMES_IN_FLIGHT = 3;
//    int NUM_TEXTURES = 16;
//    int MAX_ENTITIES = 16;
//    
//    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
//        
//    // quad
//    
//    std::vector<Vertex> vertices;
//    std::vector<uint32_t> indices;
//
//    VkBuffer vertexBuffer;
//    VkDeviceMemory vertexBufferMemory;
//    VkBuffer indexBuffer;
//    VkDeviceMemory indexBufferMemory;
//    
//    void createQuad();
//    
//    void createVertexBuffer(const VkDevice& device,
//                            const VkPhysicalDevice& physicalDevice,
//                            const VkCommandPool& commandPool,
//                            const VkQueue& graphicsQueue);
//    void createIndexBuffer(const VkDevice& device,
//                           const VkPhysicalDevice& physicalDevice,
//                           const VkCommandPool& commandPool,
//                           const VkQueue& graphicsQueue);
//    
//    GlobalUBO globalUBO;
//    std::vector<VkBuffer> globalUniformBuffers;
//    std::vector<VkDeviceMemory> globalUniformBuffersMemory;
//    std::vector<void*> globalUniformBuffersMapped;
//    
//    std::vector<InstanceSSBO> instanceSSBOs;
//    std::vector<VkBuffer> instanceStorageBuffers;
//    std::vector<VkDeviceMemory> instanceStorageBuffersMemory;
//    std::vector<void*> instanceStorageBuffersMapped;
//    
//    void createUniformBuffers(const VkDevice& device,
//                              const VkPhysicalDevice& physicalDevice);
//
//    void updateGlobalUBO(const int& windowWidth,
//                         const int& windowHeight);
//    void updateInstanceSSBOs();
//    
//    // textures
//    
//    // todo: textures should be stored in a separate system
//    
//    std::vector<VkImage> textureImages; // note: these go into swapchain
//    std::vector<VkDeviceMemory> textureImageMemories;
//    std::vector<VkImageView> textureImageViews;
//    
//    void loadTexture(const VkDevice& device,
//                     const VkPhysicalDevice& physicalDevice,
//                     const VkCommandPool& commandPool,
//                     const VkQueue& graphicsQueue,
//                     const int& texWidth,
//                     const int& texHeight,
//                     stbi_uc* pixels,
//                     const VkImageUsageFlags& usage);
//    
//    void createTexture(const VkDevice& device,
//                       const VkPhysicalDevice& physicalDevice,
//                       const VkCommandPool& commandPool,
//                       const VkQueue& graphicsQueue,
//                       const int& texWidth,
//                       const int& texHeight,
//                       const VkImageUsageFlags& usage);
//    
//    VkSampler textureSampler;
//    
//    void createTextureSampler(const VkDevice& device,
//                              const VkPhysicalDevice& physicalDevice);
//        
//    // swap chain render pipeline
//    
//    // todo: swapchain should be per window
//    
//    VkDescriptorSetLayout swapChainDescriptorSetLayout;
//    VkDescriptorPool swapChainDescriptorPool;
//    std::vector<VkDescriptorSet> swapChainDescriptorSets;
//    VkPipelineLayout swapChainPipelineLayout;
//    VkPipeline swapChainPipeline;
//    
//    void createSwapChainDescriptorPool(const VkDevice& device);
//    void createSwapChainDescriptorSetLayout(const VkDevice& device);
//    void createSwapChainDescriptorSets(const VkDevice& device);
//    void createSwapChainPipeline(const VkDevice& device,
//                                 const std::string& vertShaderPath,
//                                 const std::string& fragShaderPath,
//                                 const VkRenderPass& renderPass);
//
//    void recordSwapChainCommandBuffer(const VkCommandBuffer& commandBuffer,
//                                      const VkRenderPass& renderPass,
//                                      const VkFramebuffer& framebuffer,
//                                      const uint32_t& width,
//                                      const uint32_t& height,
//                                      const int& currentFrame);
//    
//    void cleanup(const VkDevice& device);
//    
//private:
//    // todo: move private functions here 
//        
//};
