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
    
    // ---
    
    // debugging
    
    auto debugVertShaderCode = Graphics::readFile(resolveBundlePath("debug_vert.spv"));
    auto debugFragShaderCode = Graphics::readFile(resolveBundlePath("debug_frag.spv"));

    VkShaderModule debugVertShaderModule = graphics->createShaderModule(debugVertShaderCode);
    VkShaderModule debugFragShaderModule = graphics->createShaderModule(debugFragShaderCode);

    VkPipelineShaderStageCreateInfo debugVertShaderStageInfo{};
    debugVertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    debugVertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    debugVertShaderStageInfo.module = debugVertShaderModule;
    debugVertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo debugFragShaderStageInfo{};
    debugFragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    debugFragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    debugFragShaderStageInfo.module = debugFragShaderModule;
    debugFragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo debugShaderStages[] = {
        debugVertShaderStageInfo,
        debugFragShaderStageInfo
    };

    graphics->createPipeline(debugPipeline,
                             stampDescriptorSetLayout,
                             debugPipelineLayout,
                             stampRenderPass,
                             debugShaderStages,
                             pushConstantRange);

    graphics->destroyShaderModule(debugVertShaderModule);
    graphics->destroyShaderModule(debugFragShaderModule);
    
    // ---
    
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

// ---

// debugging

void BrushEngine::calculateTiles(std::vector<BrushPoint>& brushPoints,
                                 std::vector<Tile>& tiles) {

    if (brushPoints.empty()) return;
                
    // pixels allocated left, right, top bottom
    
    int left = std::floor(canvasData.width / 2);
    int right = std::ceil(canvasData.width / 2);
    int top = std::floor(canvasData.height / 2);
    int bottom = std::ceil(canvasData.height / 2);
    
    // num tiles allocated left, right, top, bottom
    
    int maxLeft = std::ceil(left / (float) CanvasData::TILE_WIDTH);
    int maxRight = std::ceil(right / (float) CanvasData::TILE_WIDTH);
    int maxTop = std::ceil(top / (float) CanvasData::TILE_HEIGHT);
    int maxBottom = std::ceil(bottom / (float) CanvasData::TILE_HEIGHT);
    
    // middle pixel in world space
    
    glm::vec2 middle(2.f * (right / (float) canvasData.width) - 1.f,
                     2.f * (bottom / (float) canvasData.height) - 1.f);
        
    // size of tile in world space
    
    glm::vec2 tileSize(2.f * CanvasData::TILE_WIDTH / (float) canvasData.width,
                       2.f * CanvasData::TILE_HEIGHT / (float) canvasData.height);
    
    // tile map for output
    
    std::unordered_map<glm::ivec2, Tile> tileMap;
    
    for (BrushPoint& brushPoint : brushPoints) {
        
        glm::ivec2 tileIndexBottomLeft = glm::floor((brushPoint.position - brushPoint.size / 2.f - middle) / tileSize);
        glm::ivec2 tileIndexTopRight = glm::floor((brushPoint.position + brushPoint.size / 2.f - middle) / tileSize);
        
        for (int i = tileIndexBottomLeft.x; i <= tileIndexTopRight.x; i++) {
            for (int j = tileIndexBottomLeft.y; j <= tileIndexTopRight.y; j++) {
                
                glm::ivec2 tileIndex(i, j);
                
                if (tileMap.find(tileIndex) != tileMap.end()) {
                    tileMap[tileIndex].brushPoints.push_back(brushPoint);
                } else {
                    
                    glm::ivec4 dimensions;
                    dimensions.x = right + tileIndex.x * CanvasData::TILE_WIDTH;
                    dimensions.y = bottom + tileIndex.y * CanvasData::TILE_HEIGHT;
                    dimensions.z = CanvasData::TILE_WIDTH;
                    dimensions.w = CanvasData::TILE_HEIGHT;
                    
                    if (dimensions.x > canvasData.width) continue;
                    if (dimensions.y > canvasData.height) continue;
                    
                    if (dimensions.x < 0) {
                        int adjustmentValue = dimensions.x;
                        dimensions.x -= adjustmentValue;
                        dimensions.z += adjustmentValue;
                    } else if (dimensions.x + dimensions.z > canvasData.width) {
                        int adjustmentValue = canvasData.width - dimensions.x;
                        dimensions.z = adjustmentValue;
                    }
                    
                    if (dimensions.y < 0) {
                        int adjustmentValue = dimensions.y;
                        dimensions.y -= adjustmentValue;
                        dimensions.w += adjustmentValue;
                    } else if (dimensions.y + dimensions.w > canvasData.height) {
                        int adjustmentValue = canvasData.height - dimensions.y;
                        dimensions.w = adjustmentValue;
                    }
                    
                    Tile tile;
                    tile.dimensions = dimensions;
                    tile.debugColor = glm::vec4(
                        (maxLeft + tileIndex.x) / (float) (maxLeft + maxRight),
                        (maxBottom + tileIndex.y) / (float) (maxBottom + maxTop),
                        0,
                        0.15f
                    );
                    tile.brushPoints.push_back(brushPoint);
                    
                    tileMap[tileIndex] = tile;

                }
            }
        }
    }
    
    for (auto& it : tileMap) {
        tiles.push_back(it.second);
    }
    
}

void BrushEngine::recordCommandBuffer(VkCommandBuffer& commandBuffer,
                                      glm::vec2& windowSize,
                                      std::vector<Tile>& tiles) {
    
    qDebug() << "[brush engine] record command buffer";
    
    // debugging pass
    
    graphics->recordBeginRenderPass(commandBuffer,
                                    stampRenderPass,
                                    stampFrameBuffer,
                                    canvasData.width,
                                    canvasData.height,
                                    debugPipeline);

    graphics->recordSetViewport(commandBuffer,
                                0.0f,
                                0.0f,
                                canvasData.width,
                                canvasData.height);

    graphics->recordBindDescriptorSet(commandBuffer,
                                      debugPipelineLayout,
                                      stampDescriptorSet);
    
    for (Tile& tile : tiles) {
        
        if (tile.dimensions.x < 0 || tile.dimensions.y < 0) continue;
        if (tile.dimensions.x + tile.dimensions.z > canvasData.width
            || tile.dimensions.y + tile.dimensions.w > canvasData.height) continue;
        
        graphics->recordSetScissor(commandBuffer,
                                   tile.dimensions);
                
        StampPushConstant pc {
            { 0, 0 },
            { 0, 0 },
            { tile.debugColor.r, tile.debugColor.g, tile.debugColor.b, tile.debugColor.a } // range 0-1
        };

        graphics->recordPushConstant(commandBuffer,
                                     stampPipelineLayout,
                                     sizeof(pc),
                                     &pc);

        graphics->recordDraw(commandBuffer);
        
    }

    graphics->recordEndRenderPass(commandBuffer);
    
    // ---
    
    // normal pass
    
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
    
    for (Tile& tile : tiles) {

//        graphics->recordSetScissor(commandBuffer,
//                                   tile.dimensions);
        
        graphics->recordSetScissor(commandBuffer,
                                   0.0f,
                                   0.0f,
                                   canvasData.width,
                                   canvasData.height);
        
        for (BrushPoint& brushPoint : tile.brushPoints) {
                                    
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

// ---

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
    
    // debugging
    
    graphics->destroyPipeline(debugPipeline);
    graphics->destroyPipelineLayout(debugPipelineLayout);
    
    graphics->destroyPipeline(stampPipeline);
    graphics->destroyPipelineLayout(stampPipelineLayout);
    graphics->destroyRenderPass(stampRenderPass);
    graphics->destroyDescriptorPool(stampDescriptorPool);
    graphics->destroyDescriptorSetLayout(stampDescriptorSetLayout);
    
}
