//
//  graphics_system.h
//  paint
//
//  Created by Daniel Cho on 12/23/25.
//

#pragma once

#include <engine/graphics/graphics.h>

class GraphicsSystem {
    
private:
    
    // graphics constants
    
    static constexpr uint32_t TILE_WIDTH = 512;
    static constexpr uint32_t TILE_HEIGHT = 512;
    
    // paint variables
    
    int numRows, numCols;
    int firstCanvas, canvasCount;
    std::vector<glm::mat4> modelMatrices;
    
    int firstLayer, layerCount;
    
    uint32_t brushIndex;
    
    std::vector<uint32_t> brushBufferTile;
    std::vector<float> brushBufferXTile;
    std::vector<float> brushBufferYTile;
    std::vector<float> brushBufferBrushSize;

    // graphics variables
        
    VkRenderPass brushRenderPass;
    std::vector<VkFramebuffer> brushFrameBuffers;
    VkDescriptorSetLayout brushDescriptorSetLayout;
    VkDescriptorPool brushDescriptorPool;
    std::vector<VkDescriptorSet> brushDescriptorSets;
    VkPipeline brushPipeline;
    VkPipelineLayout brushPipelineLayout;
        
    VkRenderPass layerRenderPass;
    std::vector<VkFramebuffer> layerFrameBuffers;
    VkDescriptorSetLayout layerDescriptorSetLayout;
    VkDescriptorPool layerDescriptorPool;
    std::vector<VkDescriptorSet> layerDescriptorSets;
    VkPipeline layerPipeline;
    VkPipelineLayout layerPipelineLayout;
    
    // functions
    
    void initCanvas();
    void initLayer();
    void initBrushPipeline();
    void initLayerPipeline();
    
    std::array<float, 2> screenToWorldSpace(double xpos, double ypos);
    
    void recordBrushCommandBuffer(VkCommandBuffer commandBuffer,
                                  uint32_t tile,
                                  float xtile,
                                  float ytile,
                                  float brushSize);
    
public:
    
    Graphics graphics;
    
    float depth = 3.0f;
    float cameraX = 0.0f;
    float cameraY = 0.0f;
    
    void initCamera();
    
    void bufferBrush(double xpos, // screen pos
                     double ypos, // screen pos
                     float brushSize);
        
    void init();
    void draw();
    void cleanup();
    
};
