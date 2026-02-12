//
//  layer_engine.h
//  main
//
//  Created by Daniel Cho on 1/30/26.
//

#pragma once

#include <engine/graphics/graphics.h>

#include "paint/canvas/canvas_data.h"

class LayerEngine {
    
    static constexpr int MAX_LAYER_COUNT = 16;
    
public:
    LayerEngine(Graphics* graphics) : graphics(graphics) {}
    
    void init();
    void setCanvas(int canvasWidth, int canvasHeight);
    void createDescriptorSet(VkImageView& imageView, VkDescriptorSet& descriptorSet);
    void setTarget(VkImageView& imageView);
    void recordCommandBuffer(VkCommandBuffer& commandBuffer,
                             std::vector<VkDescriptorSet>& descriptorSets);
    void recordCommandBuffer(VkCommandBuffer& commandBuffer,
                             std::vector<glm::ivec2>& tiles,
                             std::vector<VkDescriptorSet>& descriptorSets);
    void cleanup();
        
private:
    Graphics* graphics;
    int canvasWidth, canvasHeight;
    
    VkRenderPass layerRenderPass;
    VkFramebuffer layerFrameBuffer;
    VkDescriptorSetLayout layerDescriptorSetLayout;
    VkDescriptorPool layerDescriptorPool;
    VkPipeline layerPipeline;
    VkPipelineLayout layerPipelineLayout;

};
