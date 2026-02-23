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
                          VK_FORMAT_R8_UNORM);
    
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
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
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
    
    // note: brush points: screen -> world
    
    const float& brushSize = brushStrokeData.brushSize;
    const float& spacing = brushStrokeData.brushSpacing;
    const glm::vec4& brushColor = brushStrokeData.color;

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
    lastInput.color = brushColor;
    
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
                glm::vec2(brushSize, brushSize),
                brushColor
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

std::unordered_map<glm::ivec2, std::vector<BrushPoint>>
BrushEngine::calculateTiles(std::vector<BrushPoint>& brushPoints) {

    if (brushPoints.empty()) return {};
    
    // todo: cache these values

    const float aspect = (float) canvasData.height / (float) canvasData.width;
    const float tileWorldW = 2.0f / (canvasData.width  / (float) CanvasData::TILE_WIDTH);
    const float tileWorldH = 2.0f * aspect / (canvasData.height / (float) CanvasData::TILE_HEIGHT);

    auto worldToTile = [&](glm::vec2 pos) -> glm::ivec2 {
        return glm::ivec2(
            glm::floor(pos.x / tileWorldW),
            glm::floor(pos.y / tileWorldH)
        );
    };

    const glm::ivec2 tileMin = worldToTile(glm::vec2(-1.0f, -aspect));
    const glm::ivec2 tileMax = worldToTile(glm::vec2( 1.0f,  aspect));
    const glm::ivec2 clampedMax = tileMax - glm::ivec2(1, 1);

    std::unordered_map<glm::ivec2, std::vector<BrushPoint>> tileMap;
        
    for (BrushPoint& brushPoint : brushPoints) {
        const glm::vec2 halfSize(brushPoint.size * 0.5f);
        const glm::vec2 bottomLeft = brushPoint.position - halfSize;
        const glm::vec2 topRight = brushPoint.position + halfSize;
        
        glm::ivec2 tileBL = worldToTile(bottomLeft);
        glm::ivec2 tileTR = worldToTile(topRight);

        tileBL = glm::clamp(tileBL, tileMin, clampedMax);
        tileTR = glm::clamp(tileTR, tileMin, clampedMax);
        
        for (int i = tileBL.x; i <= tileTR.x; i++) {
            for (int j = tileBL.y; j <= tileTR.y; j++) {
                tileMap[glm::ivec2(i, j)].push_back(brushPoint);
            }
        }
    }

    return tileMap;
}

int BrushEngine::tileEdgeX(int tileIndex) const {
    const float tileWorldW = 2.0f / (canvasData.width / (float)CanvasData::TILE_WIDTH);
    float world = tileIndex * tileWorldW;
    float clamped = std::clamp(world, -1.0f, 1.0f);
    float pixel = (clamped + 1.0f) / 2.0f * (float)canvasData.width;
    return std::clamp((int)std::round(pixel), 0, canvasData.width);
}

int BrushEngine::tileEdgeY(int tileIndex) const {
    const float aspect = (float)canvasData.height / (float)canvasData.width;
    const float tileWorldH = 2.0f * aspect / (canvasData.height / (float)CanvasData::TILE_HEIGHT);
    float world = tileIndex * tileWorldH;
    float clamped = std::clamp(world, -aspect, aspect);
    float pixel = (clamped + aspect) / (2.0f * aspect) * (float)canvasData.height;
    return std::clamp((int)std::round(pixel), 0, canvasData.height);
}

bool BrushEngine::tileToCanvas(const glm::ivec2& tileIndex, glm::ivec4& outRect) const {
    const int left   = tileEdgeX(tileIndex.x);
    const int right  = tileEdgeX(tileIndex.x + 1);
    const int bottom = tileEdgeY(tileIndex.y);
    const int top    = tileEdgeY(tileIndex.y + 1);

    outRect.x = left;
    outRect.y = bottom;
    outRect.z = right - left;
    outRect.w = top - bottom;

    return outRect.z > 0 && outRect.w > 0;
}

//bool BrushEngine::tileToCanvas(const glm::ivec2& tileIndex,
//                               glm::ivec4& outRect) const {
//    
//    // todo: cache these values
//    
//    const float aspect = (float) canvasData.height / (float) canvasData.width;
//    const float tileWorldW = 2.0f / (canvasData.width  / (float) CanvasData::TILE_WIDTH);
//    const float tileWorldH = 2.0f * aspect / (canvasData.height / (float) CanvasData::TILE_HEIGHT);
//    
//    const float worldXMin = -1.0f;
//    const float worldXMax = 1.0f;
//    const float worldYMin = -aspect;
//    const float worldYMax = aspect;
//
//    const float worldLeft = tileIndex.x * tileWorldW;
//    const float worldRight = (tileIndex.x + 1) * tileWorldW;
//    const float worldBottom = tileIndex.y * tileWorldH;
//    const float worldTop = (tileIndex.y + 1) * tileWorldH;
//
//    if (worldRight <= worldXMin || worldLeft >= worldXMax ||
//        worldTop <= worldYMin || worldBottom >= worldYMax) {
//        return false;
//    }
//
//    auto worldToPixelX = [&](float wx) {
//        return (wx - worldXMin) / (worldXMax - worldXMin) * (float)canvasData.width;
//    };
//    auto worldToPixelY = [&](float wy) {
//        return (wy - worldYMin) / (worldYMax - worldYMin) * (float)canvasData.height;
//    };
//
//    const int pixLeft = (int)std::floor(worldToPixelX(std::max(worldLeft, worldXMin)));
//    const int pixRight = (int)std::ceil( worldToPixelX(std::min(worldRight, worldXMax)));
//    const int pixBottom = (int)std::floor(worldToPixelY(std::max(worldBottom, worldYMin)));
//    const int pixTop = (int)std::ceil( worldToPixelY(std::min(worldTop, worldYMax)));
//
//    const int clampedLeft = std::clamp(pixLeft, 0, canvasData.width);
//    const int clampedRight = std::clamp(pixRight, 0, canvasData.width);
//    const int clampedBottom = std::clamp(pixBottom, 0, canvasData.height);
//    const int clampedTop = std::clamp(pixTop, 0, canvasData.height);
//
//    outRect.x = clampedLeft;
//    outRect.y = clampedBottom;
//    outRect.z = clampedRight - clampedLeft;
//    outRect.w = clampedTop - clampedBottom;
//
//    return outRect.z > 0 && outRect.w > 0;
//    
//}

void BrushEngine::recordCommandBuffer(VkCommandBuffer& commandBuffer,
                                      glm::vec2& windowSize,
                                      std::unordered_map<glm::ivec2, std::vector<BrushPoint>>& canvasMap) {
    
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
    
    for (auto& it : canvasMap) {
        
        glm::ivec4 rect;
        
        if (!tileToCanvas(it.first, rect)) continue; // out of bounds
                                
        graphics->recordSetScissor(commandBuffer,
                                   rect.x,
                                   rect.y,
                                   rect.z,
                                   rect.w);

        for (BrushPoint& brushPoint : it.second) {
            
            StampPushConstant pc {
                { brushPoint.position.x, brushPoint.position.y },
                { brushPoint.size.x, brushPoint.size.y * canvasData.aspect }, // note: account for canvas size
                { brushPoint.color.r, brushPoint.color.g, brushPoint.color.b, brushPoint.color.a } // range 0-1
            };

            graphics->recordPushConstant(commandBuffer,
                                         stampPipelineLayout,
                                         sizeof(pc),
                                         &pc);

            graphics->recordDraw(commandBuffer);

        }
        
    }

    graphics->recordEndRenderPass(commandBuffer);

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
