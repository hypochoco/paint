//
//  brush_engine.h
//  main
//
//  Created by Daniel Cho on 1/11/26.
//

#pragma once

#include <engine/graphics/graphics.h>

#include "paint/render_system/camera.h"
#include "paint/tool_system/actions/brush_stroke_data.h"
#include "paint/render_system/cache/brush_stroke_data_cache.h"
#include "paint/tool_system/actions/brush_point.h"

class BrushEngine {
    
public:
    BrushEngine(Graphics* graphics) : graphics(graphics) {}
    
    void init();
    void setCanvas(int canvasWidth, int canvasHeight);
    void createFrameBuffer(VkImageView& imageView, VkFramebuffer& frameBuffer);
    void setTarget(VkFramebuffer& frameBuffer);
    void stamp(VkCommandBuffer& commandBuffer,
               Camera& camera,
               glm::vec2& windowSize,
               BrushStrokeData& brushStrokeData,
               BrushStrokeDataCache& brushStrokeDataCache);
    void cleanup();
        
private:
    Graphics* graphics;
    int canvasWidth, canvasHeight;
    
    VkRenderPass stampRenderPass;
    VkFramebuffer stampFrameBuffer;
    VkDescriptorSetLayout stampDescriptorSetLayout;
    VkDescriptorPool stampDescriptorPool;
    VkDescriptorSet stampDescriptorSet;
    VkPipeline stampPipeline;
    VkPipelineLayout stampPipelineLayout;
    
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    std::vector<VkDeviceMemory> imageMemories;
    
    void loadBrushes();
    
    std::vector<BrushPoint> interpolate(Camera& camera,
                                        glm::vec2& windowSize,
                                        BrushStrokeData& brushStrokeData,
                                        BrushStrokeDataCache& brushStrokeDataCache);
    void recordCommandBuffer(VkCommandBuffer& commandBuffer,
                             std::vector<BrushPoint> brushPoints);
        
};
