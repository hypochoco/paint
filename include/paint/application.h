//
//  application.h
//  engine
//
//  Created by Daniel Cho on 9/30/25.
//

#pragma once

#include <engine/graphics/graphics.h>

#include "input_system.h"

// paint application

class Application {
    
private:
    
    Graphics graphics;
    
    // tiling instead
    static constexpr uint32_t CANVAS_WIDTH = 1024;
    static constexpr uint32_t CANVAS_HEIGHT = 1024;
    
    float depth;
    float brushSize;
    
    VkImage brushTextureImage;
    VkDeviceMemory brushTextureImageMemory;
    VkImageView brushTextureImageView;
    
    VkImage layerTextureImage;
    VkDeviceMemory layerTextureImageMemory;
    VkImageView layerTextureImageView;
    
    VkImage canvasTextureImage;
    VkDeviceMemory canvasTextureImageMemory;
    VkImageView canvasTextureImageView;
    
    VkRenderPass brushRenderPass;
    VkFramebuffer brushFrameBuffer;
    VkDescriptorSetLayout brushDescriptorSetLayout;
    VkDescriptorPool brushDescriptorPool;
    std::vector<VkDescriptorSet> brushDescriptorSets;
    VkPipeline brushPipeline;
    VkPipelineLayout brushPipelineLayout;
        
    VkRenderPass layerRenderPass;
    VkFramebuffer layerFrameBuffer;
    VkDescriptorSetLayout layerDescriptorSetLayout;
    VkDescriptorPool layerDescriptorPool;
    std::vector<VkDescriptorSet> layerDescriptorSets;
    VkPipeline layerPipeline;
    VkPipelineLayout layerPipelineLayout;
    
    InputSystem inputSystem;
    
    void draw();
    
public:
    
    Application();
    ~Application();

    void init();
    void run();
    void cleanup();

};
