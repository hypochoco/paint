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

void BrushEngine::setCanvas(int canvasWidth, int canvasHeight) {
    
    this->canvasWidth = canvasWidth;
    this->canvasHeight = canvasHeight;
    
}

void BrushEngine::createFrameBuffer(VkImageView& imageView, VkFramebuffer& frameBuffer) {
    
    graphics->createFramebuffer(frameBuffer,
                                stampRenderPass,
                                imageView,
                                canvasWidth,
                                canvasHeight);
        
}

void BrushEngine::setTarget(VkFramebuffer& frameBuffer) {
    
    stampFrameBuffer = frameBuffer;
        
}

std::vector<BrushPoint> BrushEngine::interpolate(Camera& camera,
                                                 glm::vec2& windowSize,
                                                 BrushStrokeData& brushStrokeData,
                                                 BrushStrokeDataCache& brushStrokeDataCache) {

    constexpr float spacing = 0.05f; // todo: move

    std::vector<BrushPoint> stamps;
    if (brushStrokeData.brushPoints.empty()) return stamps;

    float carry;
    BrushPoint lastInput;
    
    if (!brushStrokeDataCache.initialized) {
        carry = 0.0f;
        lastInput = brushStrokeData.brushPoints[0];
        lastInput.position = camera.screenToWorldSpace(windowSize, lastInput.position);
        brushStrokeDataCache.carry = carry;
        brushStrokeDataCache.currentIndex = 1;
        brushStrokeDataCache.lastInput = lastInput;
        brushStrokeDataCache.initialized = true;
    } else {
        carry = brushStrokeDataCache.carry;
        lastInput = brushStrokeDataCache.lastInput;
    }
    
    if (brushStrokeData.nextIndex == 0) { // note: base case
        stamps.push_back(lastInput);
    }

    for (size_t i = brushStrokeDataCache.currentIndex; i < brushStrokeData.brushPoints.size(); ++i) {

        BrushPoint current = brushStrokeData.brushPoints[i];
        current.position = camera.screenToWorldSpace(windowSize, current.position);

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
            stamps.push_back(
                BrushPoint{ lastInput.position + dir * traveled });
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

void BrushEngine::recordCommandBuffer(VkCommandBuffer& commandBuffer,
                                      std::vector<BrushPoint> brushPoints) {
    
    int canvasWidth = 1024; // todo: (re)move
    int canvasHeight = 1024;

    graphics->recordBeginRenderPass(commandBuffer,
                                    stampRenderPass,
                                    stampFrameBuffer,
                                    canvasWidth,
                                    canvasHeight,
                                    stampPipeline);

    graphics->recordSetViewport(commandBuffer,
                                0.0f,
                                0.0f,
                                canvasWidth,
                                canvasHeight);

    graphics->recordBindDescriptorSet(commandBuffer,
                                      stampPipelineLayout,
                                      stampDescriptorSet);

    graphics->recordSetScissor(commandBuffer,
                               0.0f,
                               0.0f,
                               canvasWidth,
                               canvasHeight);

    for (BrushPoint brushPoint : brushPoints) {

        StampPushConstant pc {
            { brushPoint.position.x, brushPoint.position.y },
            { 0.25f, 0.25f }
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
                        Camera& camera,
                        glm::vec2& windowSize,
                        BrushStrokeData& brushStrokeData,
                        BrushStrokeDataCache& brushStrokeDataCache) {

    recordCommandBuffer(commandBuffer,
                        interpolate(camera,
                                    windowSize,
                                    brushStrokeData,
                                    brushStrokeDataCache));
    
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
