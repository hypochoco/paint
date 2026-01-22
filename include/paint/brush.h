//
//  brush.hpp
//  main
//
//  Created by Daniel Cho on 1/11/26.
//

#pragma once

#include "paint/tool.h"

#include <engine/graphics/graphics.h>

struct StampPushConstant {
    float pos[2];
    float size[2];
};

class BrushEngine {
    
public:
    BrushEngine(Graphics* graphics) : graphics(graphics) {}
    
    void init();
    void cleanup();
    
//    void stamp(VkCommandBuffer& commandBuffer, std::vector<BrushPoint> stamps);
//    
//    static glm::vec2 screenToWorldSpace(float cx, float cy, float cz,
//                                        uint32_t width, uint32_t height,
//                                        float x, float y);
//    static std::vector<BrushPoint> interpolate(BrushStroke* brushStroke,
//                                               float cx, float cy, float cz,
//                                               uint32_t width, uint32_t height);
    
private:
    Graphics* graphics;
    
    VkRenderPass stampRenderPass;
    VkFramebuffer stampFrameBuffer;
    VkDescriptorSetLayout stampDescriptorSetLayout;
    VkDescriptorPool stampDescriptorPool;
    std::vector<VkDescriptorSet> stampDescriptorSets;
    VkPipeline stampPipeline;
    VkPipelineLayout stampPipelineLayout;
        
};
