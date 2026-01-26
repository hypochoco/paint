//
//  brush.hpp
//  main
//
//  Created by Daniel Cho on 1/11/26.
//

#pragma once

#include "paint/tool.h"
#include "paint/actions.h"
#include "paint/camera.h"

#include <engine/graphics/graphics.h>

struct BrushPoint; // foward declaration
struct BrushStrokeData; // foward declaration

struct StampPushConstant {
    float pos[2];
    float size[2];
};

class BrushEngine {
    
public:
    BrushEngine(Graphics* graphics) : graphics(graphics) {}
    
    void init();
    void cleanup();
    
    void stamp(VkCommandBuffer& commandBuffer,
               Camera& camera,
               glm::vec2& windowSize,
               BrushStrokeData& brushStrokeData);
        
private:
    Graphics* graphics;
    
    VkRenderPass stampRenderPass;
    VkFramebuffer stampFrameBuffer;
    VkDescriptorSetLayout stampDescriptorSetLayout;
    VkDescriptorPool stampDescriptorPool;
    std::vector<VkDescriptorSet> stampDescriptorSets;
    VkPipeline stampPipeline;
    VkPipelineLayout stampPipelineLayout;
    
    std::vector<BrushPoint> interpolate(Camera& camera,
                                        glm::vec2& windowSize,
                                        BrushStrokeData& brushStrokeData);
    void recordCommandBuffer(VkCommandBuffer& commandBuffer,
                             std::vector<BrushPoint> brushPoints);
        
};
