////
////  graphics_system.cpp
////  paint
////
////  Created by Daniel Cho on 12/23/25.
////
//
//#include "paint/graphics_system.h"
//
//#include "paint/utils.h"
//
//VkShaderModule createShaderModule(const VkDevice& device,
//                                  const std::vector<char>& code) {
//    
//    VkShaderModuleCreateInfo createInfo{};
//    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
//    createInfo.codeSize = code.size();
//    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
//
//    VkShaderModule shaderModule;
//    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
//        throw std::runtime_error("failed to create shader module!");
//    }
//
//    return shaderModule;
//    
//}
//
//uint32_t findMemoryType(const VkPhysicalDevice physicalDevice,
//                        const uint32_t& typeFilter,
//                        const VkMemoryPropertyFlags& properties) {
//    VkPhysicalDeviceMemoryProperties memProperties;
//    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
//
//    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
//        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
//            return i;
//        }
//    }
//
//    throw std::runtime_error("failed to find suitable memory type!");
//}
//
//VkCommandBuffer beginSingleTimeCommands(const VkDevice& device,
//                                        const VkCommandPool& commandPool) {
//    VkCommandBufferAllocateInfo allocInfo{};
//    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//    allocInfo.commandPool = commandPool;
//    allocInfo.commandBufferCount = 1;
//
//    VkCommandBuffer commandBuffer;
//    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
//
//    VkCommandBufferBeginInfo beginInfo{};
//    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//
//    vkBeginCommandBuffer(commandBuffer, &beginInfo);
//
//    return commandBuffer;
//}
//
//void endSingleTimeCommands(const VkDevice& device,
//                           VkCommandBuffer& commandBuffer,
//                           const VkCommandPool& commandPool,
//                           const VkQueue& graphicsQueue) {
//    
//    vkEndCommandBuffer(commandBuffer);
//
//    VkSubmitInfo submitInfo{};
//    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//    submitInfo.commandBufferCount = 1;
//    submitInfo.pCommandBuffers = &commandBuffer;
//
//    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
//    vkQueueWaitIdle(graphicsQueue);
//
//    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
//    
//}
//
//void createBuffer(const VkDevice& device,
//                  const VkPhysicalDevice physicalDevice,
//                  const VkDeviceSize& size,
//                  const VkBufferUsageFlags& usage,
//                  const VkMemoryPropertyFlags& properties,
//                  VkBuffer& buffer,
//                  VkDeviceMemory& bufferMemory) {
//    
//    VkBufferCreateInfo bufferInfo{};
//    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//    bufferInfo.size = size;
//    bufferInfo.usage = usage;
//    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
//        throw std::runtime_error("failed to create buffer!");
//    }
//
//    VkMemoryRequirements memRequirements;
//    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);
//
//    VkMemoryAllocateInfo allocInfo{};
//    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//    allocInfo.allocationSize = memRequirements.size;
//    allocInfo.memoryTypeIndex = findMemoryType(physicalDevice,
//                                               memRequirements.memoryTypeBits,
//                                               properties);
//    
//    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
//        throw std::runtime_error("failed to allocate buffer memory!");
//    }
//
//    vkBindBufferMemory(device, buffer, bufferMemory, 0);
//}
//
//void copyBuffer(const VkDevice& device,
//                const VkCommandPool& commandPool,
//                const VkQueue& graphicsQueue,
//                const VkBuffer& srcBuffer,
//                const VkBuffer& dstBuffer,
//                const VkDeviceSize& size) {
//    VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);
//
//    VkBufferCopy copyRegion{};
//    copyRegion.size = size;
//    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
//
//    endSingleTimeCommands(device, commandBuffer, commandPool, graphicsQueue);
//    
//}
//
//void copyBufferToImage(const VkDevice& device,
//                       const VkCommandPool& commandPool,
//                       const VkQueue& graphicsQueue,
//                       const VkBuffer& buffer,
//                       const VkImage& image,
//                       const uint32_t& width,
//                       const uint32_t& height) {
//    
//    VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);
//
//    VkBufferImageCopy region{};
//    region.bufferOffset = 0;
//    region.bufferRowLength = 0;
//    region.bufferImageHeight = 0;
//    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//    region.imageSubresource.mipLevel = 0;
//    region.imageSubresource.baseArrayLayer = 0;
//    region.imageSubresource.layerCount = 1;
//    region.imageOffset = {0, 0, 0};
//    region.imageExtent = {
//        width,
//        height,
//        1
//    };
//
//    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
//
//    endSingleTimeCommands(device, commandBuffer, commandPool, graphicsQueue);
//}
//
//void GraphicsSystem::createQuad() {
//    
//    vertices.reserve(vertices.size() + 4);
//
//    vertices.emplace_back( Vertex{ { -1.0f, -1.0f, 0.0f }, {1,1,1}, { 0.0f, 0.0f } } ); // bottom-left
//    vertices.emplace_back( Vertex{ {  1.0f, -1.0f, 0.0f }, {1,1,1}, { 1.0f, 0.0f } } ); // bottom-right
//    vertices.emplace_back( Vertex{ {  1.0f,  1.0f, 0.0f }, {1,1,1}, { 1.0f, 1.0f } } ); // top-right
//    vertices.emplace_back( Vertex{ { -1.0f,  1.0f, 0.0f }, {1,1,1}, { 0.0f, 1.0f } } ); // top-left
//
//    indices.insert(indices.end(), {
//        0, 1, 2,
//        2, 3, 0
//    });
//
//}
//
//void GraphicsSystem::createVertexBuffer(const VkDevice& device,
//                                        const VkPhysicalDevice& physicalDevice,
//                                        const VkCommandPool& commandPool,
//                                        const VkQueue& graphicsQueue) {
//    
//    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
//
//    VkBuffer stagingBuffer;
//    VkDeviceMemory stagingBufferMemory;
//    createBuffer(device,
//                 physicalDevice,
//                 bufferSize,
//                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//                 stagingBuffer,
//                 stagingBufferMemory);
//
//    void* data;
//    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
//    memcpy(data, vertices.data(), (size_t) bufferSize);
//    vkUnmapMemory(device, stagingBufferMemory);
//
//    createBuffer(device,
//                 physicalDevice,
//                 bufferSize,
//                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
//                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
//                 vertexBuffer,
//                 vertexBufferMemory);
//    copyBuffer(device,
//               commandPool,
//               graphicsQueue,
//               stagingBuffer,
//               vertexBuffer,
//               bufferSize);
//    
//    vkDestroyBuffer(device, stagingBuffer, nullptr);
//    vkFreeMemory(device, stagingBufferMemory, nullptr);
//}
//
//void GraphicsSystem::createIndexBuffer(const VkDevice& device,
//                                       const VkPhysicalDevice& physicalDevice,
//                                       const VkCommandPool& commandPool,
//                                       const VkQueue& graphicsQueue) {
//    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
//
//    VkBuffer stagingBuffer;
//    VkDeviceMemory stagingBufferMemory;
//    createBuffer(device,
//                 physicalDevice,
//                 bufferSize,
//                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//                 stagingBuffer,
//                 stagingBufferMemory);
//
//    void* data;
//    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
//    memcpy(data, indices.data(), (size_t) bufferSize);
//    vkUnmapMemory(device, stagingBufferMemory);
//
//    createBuffer(device,
//                 physicalDevice,
//                 bufferSize,
//                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
//                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
//                 indexBuffer,
//                 indexBufferMemory);
//
//    copyBuffer(device,
//               commandPool,
//               graphicsQueue,
//               stagingBuffer,
//               indexBuffer,
//               bufferSize);
//
//    vkDestroyBuffer(device, stagingBuffer, nullptr);
//    vkFreeMemory(device, stagingBufferMemory, nullptr);
//}
//
//void GraphicsSystem::createUniformBuffers(const VkDevice& device,
//                                          const VkPhysicalDevice& physicalDevice) {
//    
//    VkDeviceSize globalBufferSize = sizeof(GlobalUBO);
//    VkDeviceSize instanceBufferSize = sizeof(InstanceSSBO) * MAX_ENTITIES;
//    
//    globalUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
//    globalUniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
//    globalUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);
//    
//    instanceStorageBuffers.resize(MAX_FRAMES_IN_FLIGHT);
//    instanceStorageBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
//    instanceStorageBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);
//    
//    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//        
//        createBuffer(device,
//                     physicalDevice,
//                     globalBufferSize,
//                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
//                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//                     globalUniformBuffers[i],
//                     globalUniformBuffersMemory[i]);
//        vkMapMemory(device,
//                    globalUniformBuffersMemory[i],
//                    0,
//                    globalBufferSize,
//                    0,
//                    &globalUniformBuffersMapped[i]);
//        
//        createBuffer(device,
//                     physicalDevice,
//                     instanceBufferSize,
//                     VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
//                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//                     instanceStorageBuffers[i],
//                     instanceStorageBuffersMemory[i]);
//        vkMapMemory(device,
//                    instanceStorageBuffersMemory[i],
//                    0,
//                    instanceBufferSize,
//                    0,
//                    &instanceStorageBuffersMapped[i]);
//        
//    }
//    
//}
//
//void GraphicsSystem::updateGlobalUBO(const int& windowWidth,
//                                     const int& windowHeight) {
//    
//    float depth = 3.f;
//    float cameraX = 0.f;
//    float cameraY = 0.f;
//        
//    globalUBO.view = glm::lookAt(glm::vec3(cameraX, cameraY, depth), // camera pos
//                                 glm::vec3(cameraX, cameraY, 0.0f), // look at
//                                 glm::vec3(0.0f, 1.0f, 0.0f)); // up
//
//    globalUBO.proj = glm::perspective(glm::radians(45.0f), // fovy
//                                      windowWidth / (float) windowHeight, // aspect
//                                      0.1f, // near
//                                      10.0f); // far
//    globalUBO.proj[1][1] *= -1; // strange projection fix
//
//    for (uint32_t i = 0; i < (uint32_t) MAX_FRAMES_IN_FLIGHT; i++) {
//        memcpy(globalUniformBuffersMapped[i], &globalUBO, sizeof(globalUBO));
//    }
//    
//}
//
//void GraphicsSystem::updateInstanceSSBOs() {
//    
//    instanceSSBOs.assign(MAX_ENTITIES, InstanceSSBO{ glm::mat4(1.0f) });
//    
//    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//        
//        memcpy(instanceStorageBuffersMapped[i],
//               instanceSSBOs.data(),
//               instanceSSBOs.size() * sizeof(InstanceSSBO));
//
//    }
//    
//    
//    
//        
////    instanceSSBOs.reserve(MAX_ENTITIES);
////    for (uint32_t i = 0; i < MAX_ENTITIES; i++) {
////        instanceSSBOs[i] = InstanceSSBO{ glm::mat4(1.0f) };
////    }
//    
//}
//
//void createImage(const VkDevice& device,
//                 const VkPhysicalDevice& physicalDevice,
//                 const uint32_t& width,
//                 const uint32_t& height,
//                 const uint32_t& mipLevels,
//                 const VkSampleCountFlagBits& numSamples,
//                 const VkFormat& format,
//                 const VkImageTiling& tiling,
//                 const VkImageUsageFlags& usage,
//                 const VkMemoryPropertyFlags& properties,
//                 VkImage& image,
//                 VkDeviceMemory& imageMemory) {
//    
//    VkImageCreateInfo imageInfo{};
//    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
//    imageInfo.imageType = VK_IMAGE_TYPE_2D;
//    imageInfo.extent.width = width;
//    imageInfo.extent.height = height;
//    imageInfo.extent.depth = 1;
//    imageInfo.mipLevels = mipLevels;
//    imageInfo.arrayLayers = 1;
//    imageInfo.format = format;
//    imageInfo.tiling = tiling;
//    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//    imageInfo.usage = usage;
//    imageInfo.samples = numSamples;
//    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//    if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
//        throw std::runtime_error("failed to create image!");
//    }
//
//    VkMemoryRequirements memRequirements;
//    vkGetImageMemoryRequirements(device, image, &memRequirements);
//
//    VkMemoryAllocateInfo allocInfo{};
//    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//    allocInfo.allocationSize = memRequirements.size;
//    allocInfo.memoryTypeIndex = findMemoryType(physicalDevice,
//                                               memRequirements.memoryTypeBits,
//                                               properties);
//
//    if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
//        throw std::runtime_error("failed to allocate image memory!");
//    }
//
//    vkBindImageMemory(device, image, imageMemory, 0);
//}
//
//void transitionImageLayout(VkCommandBuffer& commandBuffer,
//                           const VkImage& image,
//                           const VkFormat& format,
//                           const VkImageLayout& oldLayout,
//                           const VkImageLayout& newLayout,
//                           const uint32_t& mipLevels) {
//    
//    VkImageMemoryBarrier barrier{};
//    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//    barrier.oldLayout = oldLayout;
//    barrier.newLayout = newLayout;
//    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//    barrier.image = image;
//    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//    barrier.subresourceRange.baseMipLevel = 0;
//    barrier.subresourceRange.levelCount = mipLevels;
//    barrier.subresourceRange.baseArrayLayer = 0;
//    barrier.subresourceRange.layerCount = 1;
//
//    VkPipelineStageFlags sourceStage;
//    VkPipelineStageFlags destinationStage;
//
//    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
//        && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) { // undefined -> dst optimal
//        barrier.srcAccessMask = 0;
//        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//        
//    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
//               && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) { // dst optimal -> color attachment
//        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//        destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//        
//    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
//               && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) { // dst optimal -> shader read only
//        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//        
//    } else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
//               && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) { // color attachment -> shader read only
//        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//        sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//        
//    } else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
//               && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) { // shader read only -> color attachment
//        barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
//        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//        sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//        destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//        
//    } else {
//        throw std::invalid_argument("unsupported layout transition! "
//                                    + std::to_string(static_cast<int>(oldLayout))
//                                    + " to " + std::to_string(static_cast<int>(newLayout)));
//    }
//
//    vkCmdPipelineBarrier(
//        commandBuffer,
//        sourceStage,
//        destinationStage,
//        0, 0, nullptr, 0, nullptr, 1,
//        &barrier
//    );
//
//}
//
//void transitionImageLayout(const VkDevice& device,
//                           const VkCommandPool& commandPool,
//                           const VkQueue& graphicsQueue,
//                           const VkImage& image,
//                           const VkFormat& format,
//                           const VkImageLayout& oldLayout,
//                           const VkImageLayout& newLayout,
//                           const uint32_t& mipLevels) {
//    
//    VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);
//    transitionImageLayout(commandBuffer,
//                          image,
//                          format,
//                          oldLayout,
//                          newLayout,
//                          mipLevels);
//    endSingleTimeCommands(device,
//                          commandBuffer,
//                          commandPool,
//                          graphicsQueue);
//    
//}
//
//VkImageView createImageView(const VkDevice& device,
//                            const VkImage& image,
//                            const VkFormat& format,
//                            const VkImageAspectFlags& aspectFlags,
//                            const uint32_t& mipLevels) {
//    
//    VkImageViewCreateInfo viewInfo{};
//    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//    viewInfo.image = image;
//    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
//    viewInfo.format = format;
//    viewInfo.subresourceRange.aspectMask = aspectFlags;
//    viewInfo.subresourceRange.baseMipLevel = 0;
//    viewInfo.subresourceRange.levelCount = mipLevels;
//    viewInfo.subresourceRange.baseArrayLayer = 0;
//    viewInfo.subresourceRange.layerCount = 1;
//
//    VkImageView imageView;
//    if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
//        throw std::runtime_error("failed to create image view!");
//    }
//
//    return imageView;
//    
//}
//
//void createTextureImageView(const VkDevice& device,
//                            const uint32_t& mipLevels,
//                            VkImage& textureImage,
//                            VkImageView& textureImageView) {
//    
//    textureImageView = createImageView(device,
//                                       textureImage,
//                                       VK_FORMAT_R8G8B8A8_SRGB,
//                                       VK_IMAGE_ASPECT_COLOR_BIT,
//                                       mipLevels);
//    
//}
//
//void stageTextureImage(const VkDevice& device,
//                       const VkPhysicalDevice& physicalDevice,
//                       const VkCommandPool& commandPool,
//                       const VkQueue& graphicsQueue,
//                       const int& texWidth,
//                       const int& texHeight,
//                       const VkDeviceSize& imageSize,
//                       stbi_uc* pixels,
//                       const uint32_t& mipLevels,
//                       VkImage& textureImage,
//                       VkDeviceMemory& textureImageMemory,
//                       const VkImageUsageFlags& usage) {
//        
//    VkBuffer stagingBuffer;
//    VkDeviceMemory stagingBufferMemory;
//    createBuffer(device,
//                 physicalDevice,
//                 imageSize,
//                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//                 stagingBuffer,
//                 stagingBufferMemory);
//
//    void* data;
//    vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
//        memcpy(data, pixels, static_cast<size_t>(imageSize));
//    vkUnmapMemory(device, stagingBufferMemory);
//
//    createImage(device,
//                physicalDevice,
//                texWidth,
//                texHeight,
//                mipLevels,
//                VK_SAMPLE_COUNT_1_BIT,
//                VK_FORMAT_R8G8B8A8_SRGB,
//                VK_IMAGE_TILING_OPTIMAL,
//                usage,
//                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
//                textureImage,
//                textureImageMemory);
//
//    transitionImageLayout(device,
//                          commandPool,
//                          graphicsQueue,
//                          textureImage,
//                          VK_FORMAT_R8G8B8A8_SRGB,
//                          VK_IMAGE_LAYOUT_UNDEFINED,
//                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//                          mipLevels);
//    
//    copyBufferToImage(device,
//                      commandPool,
//                      graphicsQueue,
//                      stagingBuffer,
//                      textureImage,
//                      static_cast<uint32_t>(texWidth),
//                      static_cast<uint32_t>(texHeight));
//    
//    vkDestroyBuffer(device, stagingBuffer, nullptr);
//    vkFreeMemory(device, stagingBufferMemory, nullptr);
//
//}
//
//void GraphicsSystem::loadTexture(const VkDevice& device,
//                                 const VkPhysicalDevice& physicalDevice,
//                                 const VkCommandPool& commandPool,
//                                 const VkQueue& graphicsQueue,
//                                 const int& texWidth,
//                                 const int& texHeight,
//                                 stbi_uc* pixels,
//                                 const VkImageUsageFlags& usage) {
//    
//    uint32_t mipLevels = 1;
//
//    VkImage textureImage;
//    VkDeviceMemory textureImageMemory;
//    VkImageView textureImageView;
//    
//    VkDeviceSize imageSize = texWidth * texHeight * 4; // 4 channels (RGBA)
//    
//    stageTextureImage(device,
//                      physicalDevice,
//                      commandPool,
//                      graphicsQueue,
//                      texWidth,
//                      texHeight,
//                      imageSize,
//                      pixels,
//                      mipLevels,
//                      textureImage,
//                      textureImageMemory,
//                      usage);
//    
//    transitionImageLayout(device,
//                          commandPool,
//                          graphicsQueue,
//                          textureImage,
//                          VK_FORMAT_R8G8B8A8_SRGB,
//                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
//                          1);
//    
//    createTextureImageView(device,
//                           mipLevels,
//                           textureImage,
//                           textureImageView);
//    
//    textureImages.push_back(textureImage);
//    textureImageMemories.push_back(textureImageMemory);
//    textureImageViews.push_back(textureImageView);
//    
//}
//
//void GraphicsSystem::createTexture(const VkDevice& device,
//                                   const VkPhysicalDevice& physicalDevice,
//                                   const VkCommandPool& commandPool,
//                                   const VkQueue& graphicsQueue,
//                                   const int& texWidth,
//                                   const int& texHeight,
//                                   const VkImageUsageFlags& usage) {
//    
//    VkDeviceSize imageSize = texWidth * texHeight * 4; // 4 channels (RGBA)
//    stbi_uc* pixels = new stbi_uc[imageSize];
//    std::fill(pixels, pixels + imageSize, 255); // fill white
//    
//    loadTexture(device,
//                physicalDevice,
//                commandPool,
//                graphicsQueue,
//                texWidth,
//                texHeight,
//                pixels,
//                usage);
//    
//    stbi_image_free(pixels);
//    
//}
//
//void GraphicsSystem::createTextureSampler(const VkDevice& device,
//                                          const VkPhysicalDevice& physicalDevice) {
//    
//    VkPhysicalDeviceProperties properties{};
//    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
//
//    VkSamplerCreateInfo samplerInfo{};
//    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
//    samplerInfo.magFilter = VK_FILTER_LINEAR;
//    samplerInfo.minFilter = VK_FILTER_LINEAR;
//    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
//    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
//    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
//    samplerInfo.anisotropyEnable = VK_TRUE;
//    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
//    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
//    samplerInfo.unnormalizedCoordinates = VK_FALSE;
//    samplerInfo.compareEnable = VK_FALSE;
//    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
//    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
//    samplerInfo.minLod = 0.0f;
//    samplerInfo.maxLod = VK_LOD_CLAMP_NONE;
//    samplerInfo.mipLodBias = 0.0f;
//
//    if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
//        throw std::runtime_error("failed to create texture sampler!");
//    }
//    
//}
//
//void GraphicsSystem::createSwapChainDescriptorPool(const VkDevice& device) {
//    
//    // device
//    
//    std::array<VkDescriptorPoolSize, 3> poolSizes{};
//    poolSizes[0] = VkDescriptorPoolSize{
//        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
//        static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) };
//    poolSizes[1] = VkDescriptorPoolSize{
//        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
//        static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) };
//    poolSizes[2] = VkDescriptorPoolSize{
//        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
//        static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * NUM_TEXTURES) };
//
//    VkDescriptorPoolCreateInfo poolInfo{};
//    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
//    poolInfo.pPoolSizes = poolSizes.data();
//    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
//
//    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &swapChainDescriptorPool) != VK_SUCCESS) {
//        qFatal("failed to create descriptor pool!");
//    }
//    
//}
//
//void GraphicsSystem::createSwapChainDescriptorSetLayout(const VkDevice& device) {
//    
//    // device
//    
//    // global UBO
//    VkDescriptorSetLayoutBinding globalUBOLayoutBinding{};
//    globalUBOLayoutBinding.binding = 0;
//    globalUBOLayoutBinding.descriptorCount = 1;
//    globalUBOLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//    globalUBOLayoutBinding.pImmutableSamplers = nullptr;
//    globalUBOLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//
//    // instance SSBO's
//    VkDescriptorSetLayoutBinding instanceSSBOLayoutBinding{};
//    instanceSSBOLayoutBinding.binding = 1;
//    instanceSSBOLayoutBinding.descriptorCount = 1;
//    instanceSSBOLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//    instanceSSBOLayoutBinding.pImmutableSamplers = nullptr;
//    instanceSSBOLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//
//    // texture array
//    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
//    samplerLayoutBinding.binding = 2;
//    samplerLayoutBinding.descriptorCount = NUM_TEXTURES;
//    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//    samplerLayoutBinding.pImmutableSamplers = nullptr;
//    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
//    
//    std::array<VkDescriptorSetLayoutBinding, 3> bindings = {
//        globalUBOLayoutBinding, instanceSSBOLayoutBinding, samplerLayoutBinding
//    };
//
//    VkDescriptorSetLayoutCreateInfo layoutInfo{};
//    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
//    layoutInfo.pBindings = bindings.data();
//
//    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &swapChainDescriptorSetLayout) != VK_SUCCESS) {
//        qFatal("failed to create descriptor set layout!");
//    }
//
//}
//
//void GraphicsSystem::createSwapChainDescriptorSets(const VkDevice& device) {
//    
//    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT,
//                                               swapChainDescriptorSetLayout);
//    
//    VkDescriptorSetAllocateInfo allocInfo{};
//    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//    allocInfo.descriptorPool = swapChainDescriptorPool;
//    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
//    allocInfo.pSetLayouts = layouts.data();
//    
//    swapChainDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
//    if (vkAllocateDescriptorSets(device, &allocInfo, swapChainDescriptorSets.data()) != VK_SUCCESS) {
//        throw std::runtime_error("failed to allocate descriptor sets!");
//    }
//    
//    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//        VkDescriptorBufferInfo globalInfo{};
//        globalInfo.buffer = globalUniformBuffers[i];
//        globalInfo.offset = 0;
//        globalInfo.range  = sizeof(GlobalUBO);
//        
//        VkDescriptorBufferInfo instanceInfo{};
//        instanceInfo.buffer = instanceStorageBuffers[i];
//        instanceInfo.offset = 0;
//        instanceInfo.range  = sizeof(InstanceSSBO) * MAX_ENTITIES;
//        
//        std::vector<VkDescriptorImageInfo> imageInfos(NUM_TEXTURES);
//        
//        for (size_t j = 0; j < NUM_TEXTURES; j++) {
//            imageInfos[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//            imageInfos[j].imageView = textureImageViews[std::min(j, textureImageViews.size() - 1)];
//            imageInfos[j].sampler = textureSampler;
//        }
//        
//        std::array<VkWriteDescriptorSet, 3> descriptorWrites{};
//        
//        // global uniform bugger
//        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//        descriptorWrites[0].dstSet = swapChainDescriptorSets[i];
//        descriptorWrites[0].dstBinding = 0;
//        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//        descriptorWrites[0].descriptorCount = 1;
//        descriptorWrites[0].pBufferInfo = &globalInfo;
//        
//        // ssbo's
//        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//        descriptorWrites[1].dstSet = swapChainDescriptorSets[i];
//        descriptorWrites[1].dstBinding = 1;
//        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//        descriptorWrites[1].descriptorCount = 1;
//        descriptorWrites[1].pBufferInfo = &instanceInfo;
//        
//        // textures
//        descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//        descriptorWrites[2].dstSet = swapChainDescriptorSets[i];
//        descriptorWrites[2].dstBinding = 2;
//        descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//        descriptorWrites[2].descriptorCount = static_cast<uint32_t>(imageInfos.size());
//        descriptorWrites[2].pImageInfo = imageInfos.data();
//        
//        vkUpdateDescriptorSets(device,
//                               static_cast<uint32_t>(descriptorWrites.size()),
//                               descriptorWrites.data(),
//                               0,
//                               nullptr);
//        
//    }
//    
//}
//    
//void GraphicsSystem::createSwapChainPipeline(const VkDevice& device,
//                                             const std::string& vertShaderPath,
//                                             const std::string& fragShaderPath,
//                                             const VkRenderPass& renderPass) {
//    
//    // devicea
//    // shader paths
//    // vertex
//    
//    auto vertShaderCode = readFile(vertShaderPath);
//    auto fragShaderCode = readFile(fragShaderPath);
//    
//    VkShaderModule vertShaderModule = createShaderModule(device, vertShaderCode);
//    VkShaderModule fragShaderModule = createShaderModule(device, fragShaderCode);
//    
//    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
//    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
//    vertShaderStageInfo.module = vertShaderModule;
//    vertShaderStageInfo.pName = "main";
//    
//    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
//    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
//    fragShaderStageInfo.module = fragShaderModule;
//    fragShaderStageInfo.pName = "main";
//    
//    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
//    
//    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
//    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//    
//    auto bindingDescription = Vertex::getBindingDescription();
//    auto attributeDescriptions = Vertex::getAttributeDescriptions();
//    
//    vertexInputInfo.vertexBindingDescriptionCount = 1;
//    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
//    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
//    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
//    
//    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
//    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
//    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
//    inputAssembly.primitiveRestartEnable = VK_FALSE;
//    
//    VkPipelineViewportStateCreateInfo viewportState{};
//    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
//    viewportState.viewportCount = 1;
//    viewportState.scissorCount = 1;
//    
//    VkPipelineRasterizationStateCreateInfo rasterizer{};
//    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
//    rasterizer.depthClampEnable = VK_FALSE;
//    rasterizer.rasterizerDiscardEnable = VK_FALSE;
//    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
//    rasterizer.lineWidth = 1.0f;
//    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
//    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
//    rasterizer.depthBiasEnable = VK_FALSE;
//    
//    VkPipelineMultisampleStateCreateInfo multisampling{};
//    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
//    multisampling.sampleShadingEnable = VK_FALSE;
//    multisampling.rasterizationSamples = msaaSamples;
//    
//    VkPipelineDepthStencilStateCreateInfo depthStencil{};
//    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
//    depthStencil.depthTestEnable = VK_TRUE;
//    depthStencil.depthWriteEnable = VK_TRUE;
//    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
//    depthStencil.depthBoundsTestEnable = VK_FALSE;
//    depthStencil.stencilTestEnable = VK_FALSE;
//    
//    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
//    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
//    | VK_COLOR_COMPONENT_G_BIT
//    | VK_COLOR_COMPONENT_B_BIT
//    | VK_COLOR_COMPONENT_A_BIT;
//    colorBlendAttachment.blendEnable = VK_FALSE;
//    
//    VkPipelineColorBlendStateCreateInfo colorBlending{};
//    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
//    colorBlending.logicOpEnable = VK_FALSE;
//    colorBlending.logicOp = VK_LOGIC_OP_COPY;
//    colorBlending.attachmentCount = 1;
//    colorBlending.pAttachments = &colorBlendAttachment;
//    colorBlending.blendConstants[0] = 0.0f;
//    colorBlending.blendConstants[1] = 0.0f;
//    colorBlending.blendConstants[2] = 0.0f;
//    colorBlending.blendConstants[3] = 0.0f;
//    
//    std::vector<VkDynamicState> dynamicStates = {
//        VK_DYNAMIC_STATE_VIEWPORT,
//        VK_DYNAMIC_STATE_SCISSOR
//    };
//    VkPipelineDynamicStateCreateInfo dynamicState{};
//    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
//    dynamicState.pDynamicStates = dynamicStates.data();
//    
//    VkPushConstantRange pushConstantRange{};
//    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
//    pushConstantRange.offset = 0;
//    pushConstantRange.size = sizeof(uint32_t) * 2; // first material index + count
//    
//    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
//    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//    pipelineLayoutInfo.setLayoutCount = 1;
//    pipelineLayoutInfo.pSetLayouts = &swapChainDescriptorSetLayout;
//    
//    pipelineLayoutInfo.pushConstantRangeCount = 1;
//    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
//    
//    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &swapChainPipelineLayout) != VK_SUCCESS) {
//        throw std::runtime_error("failed to create pipeline layout!");
//    }
//    
//    VkGraphicsPipelineCreateInfo pipelineInfo{};
//    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//    pipelineInfo.stageCount = 2;
//    pipelineInfo.pStages = shaderStages;
//    pipelineInfo.pVertexInputState = &vertexInputInfo;
//    pipelineInfo.pInputAssemblyState = &inputAssembly;
//    pipelineInfo.pViewportState = &viewportState;
//    pipelineInfo.pRasterizationState = &rasterizer;
//    pipelineInfo.pMultisampleState = &multisampling;
//    pipelineInfo.pDepthStencilState = &depthStencil;
//    pipelineInfo.pColorBlendState = &colorBlending;
//    pipelineInfo.pDynamicState = &dynamicState;
//    pipelineInfo.layout = swapChainPipelineLayout;
//    pipelineInfo.renderPass = renderPass;
//    pipelineInfo.subpass = 0;
//    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
//    
//    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &swapChainPipeline) != VK_SUCCESS) {
//        qFatal("failed to create graphics pipeline!");
//    }
//    
//    vkDestroyShaderModule(device, fragShaderModule, nullptr);
//    vkDestroyShaderModule(device, vertShaderModule, nullptr);
//    
//}
//    
//void GraphicsSystem::recordSwapChainCommandBuffer(const VkCommandBuffer& commandBuffer,
//                                                  const VkRenderPass& renderPass,
//                                                  const VkFramebuffer& framebuffer,
//                                                  const uint32_t& width,
//                                                  const uint32_t& height,
//                                                  const int& currentFrame) {
//    
//    // command buffer
//    // frame buffer
//    // swapchain extent
//    // current frame - for descriptor set
//    
//    VkRenderPassBeginInfo renderPassInfo{};
//    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//    renderPassInfo.renderPass = renderPass;
//    renderPassInfo.framebuffer = framebuffer;
//    renderPassInfo.renderArea.offset = {0, 0};
//    renderPassInfo.renderArea.extent = { width, height }; // todo
//    
//    std::array<VkClearValue, 2> clearValues{};
//    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
//    clearValues[1].depthStencil = {1.0f, 0};
//    
//    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
//    renderPassInfo.pClearValues = clearValues.data();
//    
//    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
//    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, swapChainPipeline);
//    
//    VkViewport viewport{};
//    viewport.x = 0.0f;
//    viewport.y = 0.0f;
//    viewport.width = (float) width;
//    viewport.height = (float) height;
//    viewport.minDepth = 0.0f;
//    viewport.maxDepth = 1.0f;
//    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
//    
//    VkRect2D scissor{};
//    scissor.offset = { 0, 0 };
//    scissor.extent = { width, height };
//    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
//    
//    VkBuffer vertexBuffers[] = { vertexBuffer };
//    VkDeviceSize offsets[] = { 0 };
//    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
//    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
//    
//    vkCmdBindDescriptorSets(
//                            commandBuffer,
//                            VK_PIPELINE_BIND_POINT_GRAPHICS,
//                            swapChainPipelineLayout,
//                            0,
//                            1,
//                            &swapChainDescriptorSets[currentFrame],
//                            0,
//                            nullptr
//                            );
//    
//    struct MaterialPushConstant { uint32_t firstMaterial; uint32_t materialCount; } pc;
//    pc.firstMaterial = 0;
//    pc.materialCount = 1;
//    
//    vkCmdPushConstants(commandBuffer,
//                       swapChainPipelineLayout,
//                       VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
//                       0,
//                       sizeof(pc),
//                       &pc);
//        
//    vkCmdDrawIndexed(commandBuffer,
//                     (uint32_t) indices.size(),
//                     1,
//                     0,
//                     0,
//                     0);
//    
//    vkCmdEndRenderPass(commandBuffer);
//    
//}
//    
//void GraphicsSystem::cleanup(const VkDevice& device) {
//    
//    vkDestroyPipeline(device, swapChainPipeline, nullptr);
//    vkDestroyPipelineLayout(device, swapChainPipelineLayout, nullptr);
//    
//    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//        vkDestroyBuffer(device, globalUniformBuffers[i], nullptr);
//        vkFreeMemory(device, globalUniformBuffersMemory[i], nullptr);
//        
//        vkDestroyBuffer(device, instanceStorageBuffers[i], nullptr);
//        vkFreeMemory(device, instanceStorageBuffersMemory[i], nullptr);
//    }
//
//    vkDestroyDescriptorPool(device, swapChainDescriptorPool, nullptr);
//
//    vkDestroySampler(device, textureSampler, nullptr);
//    
//    for (auto& tiv : textureImageViews) {
//        vkDestroyImageView(device, tiv, nullptr);
//    }
//    for (auto& ti : textureImages) {
//        vkDestroyImage(device, ti, nullptr);
//    }
//    for (auto& tim : textureImageMemories) {
//        vkFreeMemory(device, tim, nullptr);
//    }
//    
//    vkDestroyDescriptorSetLayout(device, swapChainDescriptorSetLayout, nullptr);
//    
//    vkDestroyBuffer(device, indexBuffer, nullptr);
//    vkFreeMemory(device, indexBufferMemory, nullptr);
//    vkDestroyBuffer(device, vertexBuffer, nullptr);
//    vkFreeMemory(device, vertexBufferMemory, nullptr);
//    
//}
