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
#include "paint/canvas/canvas_data.h"

class BrushEngine {
    
public:
    BrushEngine(Graphics* graphics) : graphics(graphics) {}
    
    void init();
    void setCanvasData(CanvasData canvasData);
    void createFrameBuffer(VkImageView& imageView, VkFramebuffer& frameBuffer);
    void setTarget(VkFramebuffer& frameBuffer);
    std::vector<BrushPoint> interpolate(Camera& camera,
                                        glm::vec2& windowSize,
                                        BrushStrokeData& brushStrokeData,
                                        BrushStrokeDataCache& brushStrokeDataCache);
    std::unordered_map<glm::ivec2, std::vector<BrushPoint>>
    calculateTile(std::vector<BrushPoint>& brushPoints);
    glm::ivec2 tileToCanvas(glm::ivec2 tile);
    void recordCommandBuffer(VkCommandBuffer& commandBuffer,
                             std::unordered_map<glm::ivec2, std::vector<BrushPoint>>& canvasMap);
    void cleanup();
        
private:
    Graphics* graphics;
    
    CanvasData canvasData;
    
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
    
};
