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
    
    const float& brushSize = brushStrokeData.brushSize;
    const float& spacing = brushStrokeData.brushSpacing;

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

std::unordered_map<glm::ivec2, std::vector<BrushPoint>>
BrushEngine::calculateTile(std::vector<BrushPoint>& brushPoints) {
    
    // note: canvas space actually
    // note: world space calculation, coordinate system
    // note: assumes x coord -1 -> 1
    
    // todo: different output structure, list of tiles
    // todo: cleanup the brushpoints, make separate screenpoints and world points
    
    std::unordered_map<glm::ivec2, std::vector<BrushPoint>> tileMap;
    
    if (brushPoints.size() == 0) return tileMap;
    
    glm::vec2 tileValue(canvasData.width / (float) CanvasData::TILE_WIDTH,
                         canvasData.height / (float) CanvasData::TILE_HEIGHT);
    tileValue /= 2.f; // note: account for origin
    glm::ivec2 tileMin = glm::ivec2(glm::ceil(-tileValue));
    glm::ivec2 tileMax = glm::ivec2(glm::ceil(tileValue));
            
    for (BrushPoint& brushPoint : brushPoints) {
        
        glm::vec2 bottomLeft = brushPoint.position - 0.5f * brushPoint.size;
        glm::vec2 topRight = brushPoint.position + 0.5f * brushPoint.size;
        
        glm::ivec2 bottomLeftTile = glm::ivec2(glm::floor(tileValue * bottomLeft));
        glm::ivec2 topRightTile = glm::ivec2(glm::floor(tileValue * topRight));
        
//        qDebug() << "[brush engine] bottom left:"
//        << "\n\tposition" << bottomLeft.x << ", " << bottomLeft.y
//        << "\n\traw tile" << (tileValue * bottomLeft).x << ", " << (tileValue * bottomLeft).y
//        << "\n\ttile" << bottomLeftTile.x << ", " << bottomLeftTile.y;
//        
//        qDebug() << "[brush engine] top right:"
//        << "\n\tposition" << topRightTile.x << ", " << topRightTile.y
//        << "\n\traw tile" << (tileValue * topRight).x << ", " << (tileValue * topRight).y
//        << "\n\ttile" << topRightTile.x << ", " << topRightTile.y;
         
        for (int i = std::max(bottomLeftTile.x, tileMin.x); i <= std::min(topRightTile.x, tileMax.x); i++) {
            for (int j = std::max(bottomLeftTile.y, tileMin.y); j <= std::min(topRightTile.y, tileMax.y); j++) {
                tileMap[glm::ivec2(i, j)].push_back(brushPoint);
            }
        }
    }
    
//    qDebug() << "[brush engine] calculated tiles: " << tiles.size();
//    for (auto& tile : tiles) {
//        qDebug() << "\t" << tile.x << ", " << tile.y;
//    }
    
    return tileMap;
    
}

glm::ivec2 BrushEngine::tileToCanvas(glm::ivec2 tile) {
    
    // todo: what about the odd case ?
    
    glm::ivec2 center(canvasData.width / 2, canvasData.height / 2);
    
    return center + tile * glm::ivec2(CanvasData::TILE_WIDTH, CanvasData::TILE_HEIGHT);
    
}

void BrushEngine::recordCommandBuffer(VkCommandBuffer& commandBuffer,
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
                
        graphics->recordSetScissor(commandBuffer, // todo: does this actually work ?
                                   it.first.x,
                                   it.first.y,
                                   CanvasData::TILE_WIDTH,
                                   CanvasData::TILE_HEIGHT);

        for (BrushPoint& brushPoint : it.second) {
            
            StampPushConstant pc {
                { brushPoint.position.x, brushPoint.position.y },
                { brushPoint.size.x, brushPoint.size.y * canvasData.aspect },
                { 1, 0, 1, 1 }
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
