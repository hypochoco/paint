//
//  layer_engine.h
//  main
//
//  Created by Daniel Cho on 1/30/26.
//

#pragma once

#include <engine/graphics/graphics.h>

class LayerEngine {
    
    static constexpr int MAX_LAYER_COUNT = 16;
    
public:
    LayerEngine(Graphics* graphics) : graphics(graphics) {}
    
    void init();
    void setCanvas(int canvasWidth, int canvasHeight);
    void createDescriptorSet(VkImageView& imageView, VkDescriptorSet& descriptorSet);
    void setSource(VkDescriptorSet& descriptorSet);
    void setTarget(VkImageView& imageView);
    void stamp(VkCommandBuffer& commandBuffer);
    void cleanup();
        
private:
    Graphics* graphics;
    int canvasWidth, canvasHeight;
    
    VkRenderPass layerRenderPass;
    VkFramebuffer layerFrameBuffer;
    VkDescriptorSetLayout layerDescriptorSetLayout;
    VkDescriptorPool layerDescriptorPool;
    VkDescriptorSet layerDescriptorSet;
    VkPipeline layerPipeline;
    VkPipelineLayout layerPipelineLayout;

};
