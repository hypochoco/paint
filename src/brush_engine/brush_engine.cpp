//
//  brush_engine.cpp
//  main
//
//  Created by Daniel Cho on 1/11/26.
//

#include "paint/brush_engine/brush_engine.h"

#include "paint/brush_engine/stamp_push_constant.h"

#include "paint/utils.h"

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
    
    // temp: default texture stamp

    graphics->loadTexture(resolveBundlePath("brush.png"),
                          VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                          | VK_IMAGE_USAGE_TRANSFER_DST_BIT
                          | VK_IMAGE_USAGE_SAMPLED_BIT,
                          1);

    graphics->createDescriptorSets(graphics->textureImageViews[1], // temp
                                   stampDescriptorSets,
                                   stampDescriptorSetLayout,
                                   stampDescriptorPool);

    graphics->createFramebuffer(stampFrameBuffer,
                                stampRenderPass,
                                graphics->textureImageViews[0], // temp
                                (int) 1024, // canvas width
                                (int) 1024); // canvas height

}

glm::vec2 screenToWorldSpace(glm::vec3 position,
                             glm::vec2 windowSize,
                             glm::vec2 point) {
    
    float aspect = windowSize.x / windowSize.y; // window aspect
    static float tanHalfFovy = 0.4142f; // hard coded for 45 deg
    
    float ndcX = (point.x / windowSize.x) * 2.0f - 1.0f;
    float ndcY = 1.0f - (point.y / windowSize.y) * 2.0f;

    return {
        ndcX * position.z * tanHalfFovy * aspect + position.x,
        ndcY * position.z * tanHalfFovy + position.y
    };

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
        lastInput.position = screenToWorldSpace(camera.position,
                                                windowSize,
                                                lastInput.position);
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
        current.position = screenToWorldSpace(camera.position,
                                              windowSize,
                                              current.position);

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
                                      stampDescriptorSets);

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
    
    graphics->destroyPipeline(stampPipeline);
    graphics->destroyPipelineLayout(stampPipelineLayout);
    graphics->destroyRenderPass(stampRenderPass);
    graphics->destroyFrameBuffer(stampFrameBuffer);
    graphics->destroyDescriptorPool(stampDescriptorPool);
    graphics->destroyDescriptorSetLayout(stampDescriptorSetLayout);
    
}
