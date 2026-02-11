//
//  brush_engine.cpp
//  main
//
//  Created by Daniel Cho on 1/11/26.
//

#include "paint/brush_engine/brush_engine.h"

#include "paint/brush_engine/stamp_push_constant.h"

#include "paint/utils.h"

void BrushEngine::loadBrushes() {
    
    // temp: default texture stamp
    
    VkImage image;
    VkDeviceMemory imageMemory;
    VkImageView imageView;

    graphics->loadTexture(resolveBundlePath("brush.png"),
                          image,
                          imageMemory,
                          imageView,
                          VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                          | VK_IMAGE_USAGE_TRANSFER_DST_BIT
                          | VK_IMAGE_USAGE_SAMPLED_BIT,
                          1);
    
    graphics->createDescriptorSet(imageView,
                                  stampDescriptorSet,
                                  stampDescriptorSetLayout,
                                  stampDescriptorPool);
    
    images.push_back(image);
    imageMemories.push_back(imageMemory);
    imageViews.push_back(imageView);

}

void BrushEngine::init() {
    
    graphics->createRenderPass(stampRenderPass,
                               VK_ATTACHMENT_LOAD_OP_LOAD);

    graphics->createDescriptorSetLayout(stampDescriptorSetLayout);
    graphics->createDescriptorPool(stampDescriptorPool);

    auto stampVertShaderCode = Graphics::readFile(resolveBundlePath("texture_stamp_vert.spv"));
    auto stampFragShaderCode = Graphics::readFile(resolveBundlePath("texture_stamp_frag.spv"));

    VkShaderModule stampVertShaderModule = graphics->createShaderModule(stampVertShaderCode);
    VkShaderModule stampFragShaderModule = graphics->createShaderModule(stampFragShaderCode);

    VkPipelineShaderStageCreateInfo stampVertShaderStageInfo{};
    stampVertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stampVertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    stampVertShaderStageInfo.module = stampVertShaderModule;
    stampVertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo stampFragShaderStageInfo{};
    stampFragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stampFragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    stampFragShaderStageInfo.module = stampFragShaderModule;
    stampFragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo stampShaderStages[] = { stampVertShaderStageInfo, stampFragShaderStageInfo };

    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(StampPushConstant);

    graphics->createPipeline(stampPipeline,
                             stampDescriptorSetLayout,
                             stampPipelineLayout,
                             stampRenderPass,
                             stampShaderStages,
                             pushConstantRange);

    graphics->destroyShaderModule(stampVertShaderModule);
    graphics->destroyShaderModule(stampFragShaderModule);
    
    loadBrushes();
    
}

void BrushEngine::setCanvasData(CanvasData canvasData) {
    
    this->canvasData = canvasData;
    
}

void BrushEngine::createFrameBuffer(VkImageView& imageView, VkFramebuffer& frameBuffer) {
    
    graphics->createFramebuffer(frameBuffer,
                                stampRenderPass,
                                imageView,
                                canvasData.width,
                                canvasData.height);
        
}

void BrushEngine::setTarget(VkFramebuffer& frameBuffer) {
    
    stampFrameBuffer = frameBuffer;
        
}

std::vector<BrushPoint> BrushEngine::interpolate(Camera& camera,
                                                 glm::vec2& windowSize,
                                                 BrushStrokeData& brushStrokeData,
                                                 BrushStrokeDataCache& brushStrokeDataCache) {

    constexpr float brushSize = 0.25f; // todo: move
    constexpr float spacing = 0.05f; // todo: move

    std::vector<BrushPoint> stamps;
    if (brushStrokeData.brushPoints.empty()) return stamps;

    float carry;
    BrushPoint lastInput;
    
    if (!brushStrokeDataCache.initialized) {
        carry = 0.0f;
        lastInput = brushStrokeData.brushPoints[0];
        lastInput.position = camera.screenToWorldSpace(windowSize, lastInput.position);
        lastInput.position.x /= canvasData.aspect; // note: account for canvas size
        brushStrokeDataCache.carry = carry;
        brushStrokeDataCache.currentIndex = 1;
        brushStrokeDataCache.lastInput = lastInput;
        brushStrokeDataCache.initialized = true;
    } else {
        carry = brushStrokeDataCache.carry;
        lastInput = brushStrokeDataCache.lastInput;
    }
    
    lastInput.size = glm::vec2(brushSize, brushSize);
    
    if (brushStrokeData.nextIndex == 0) { // note: base case
        stamps.push_back(lastInput);
    }

    for (size_t i = brushStrokeDataCache.currentIndex; i < brushStrokeData.brushPoints.size(); ++i) {

        BrushPoint current = brushStrokeData.brushPoints[i];
        current.position = camera.screenToWorldSpace(windowSize, current.position);
        current.position.x /= canvasData.aspect; // note: account for canvas size
        
        glm::vec2 segment = current.position - lastInput.position;
        float segLen = glm::length(segment);

        if (segLen == 0.0f) {
            lastInput = current;
            continue;
        }

        glm::vec2 dir = segment / segLen;
        float traveled = 0.0f;

        while (traveled + spacing - carry <= segLen) {
            float step = spacing - carry;
            traveled += step;
            stamps.push_back(BrushPoint{
                lastInput.position + dir * traveled,
                glm::vec2(brushSize, brushSize)
            });
            carry = 0.0f;
        }

        carry += segLen - traveled;
        lastInput = current;

        brushStrokeDataCache.currentIndex = i + 1;
        brushStrokeDataCache.lastInput = lastInput;
    }

    brushStrokeDataCache.carry = carry;
    return stamps;
}

void BrushEngine::calculateTile(std::vector<BrushPoint>& brushPoints) {
    
    // note: canvas space actually
    
    // note: world space calculation, coordinate system
    // note: assumes x coord -1 -> 1
    
    // todo: different output structure, list of tiles
    
    glm::vec2 tile_value(canvasData.width / (float) CanvasData::TILE_WIDTH,
                         canvasData.height / (float) CanvasData::TILE_HEIGHT);
    tile_value /= 2.f; // note: account for origin
        
    for (BrushPoint& brushPoint : brushPoints) {
        
        glm::vec2 bottomLeft = brushPoint.position + brushPoint.size;
        glm::vec2 topRight = brushPoint.position - brushPoint.size;
                        
        glm::vec2 tile_position = tile_value * brushPoint.position;

        qDebug() << "[brush engine] tile "
        << "\n\ttile value: " << tile_value.x << ", " << tile_value.y
        << "\n\tbrush point: " << brushPoint.position.x << ", " << brushPoint.position.y
        << "\n\ttile position" << tile_position.x << ", " << tile_position.y
        << "\n\ttile position" << std::floor(tile_position.x) << ", " << std::floor(tile_position.y);
        
        // todo: cleanup the brushpoints, make separate screenpoints and world points
        
        // todo: do something with this
        
    }
    
}

void BrushEngine::recordCommandBuffer(VkCommandBuffer& commandBuffer,
                                      std::vector<BrushPoint>& brushPoints) {
    
    graphics->recordBeginRenderPass(commandBuffer,
                                    stampRenderPass,
                                    stampFrameBuffer,
                                    canvasData.width,
                                    canvasData.height,
                                    stampPipeline);

    graphics->recordSetViewport(commandBuffer,
                                0.0f,
                                0.0f,
                                canvasData.width,
                                canvasData.height);

    graphics->recordBindDescriptorSet(commandBuffer,
                                      stampPipelineLayout,
                                      stampDescriptorSet);

    graphics->recordSetScissor(commandBuffer,
                               0.0f,
                               0.0f,
                               canvasData.width,
                               canvasData.height);

    for (BrushPoint& brushPoint : brushPoints) {
        
        StampPushConstant pc {
            { brushPoint.position.x, brushPoint.position.y },
            { brushPoint.size.x, brushPoint.size.y * canvasData.aspect }
        };

        graphics->recordPushConstant(commandBuffer,
                                     stampPipelineLayout,
                                     sizeof(pc),
                                     &pc);

        graphics->recordDraw(commandBuffer);

    }

    graphics->recordEndRenderPass(commandBuffer);

}

void BrushEngine::stamp(VkCommandBuffer& commandBuffer,
                        std::vector<BrushPoint>& brushPoints) {
        
    // calculateTile(brushPoints);

    recordCommandBuffer(commandBuffer,
                        brushPoints);
    
}

void BrushEngine::cleanup() {
    
    for (auto& iv : imageViews) {
        graphics->destroyVkImageView(iv);
    }
    for (auto& i : images) {
        graphics->destroyVkImage(i);
    }
    for (auto& im : imageMemories) {
        graphics->destroyVkDeviceMemory(im);
    }
    
    graphics->destroyPipeline(stampPipeline);
    graphics->destroyPipelineLayout(stampPipelineLayout);
    graphics->destroyRenderPass(stampRenderPass);
    graphics->destroyDescriptorPool(stampDescriptorPool);
    graphics->destroyDescriptorSetLayout(stampDescriptorSetLayout);
    
}
