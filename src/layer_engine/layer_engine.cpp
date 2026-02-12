//
//  layer_engine.cpp
//  main
//
//  Created by Daniel Cho on 1/30/26.
//

#include "paint/layer_engine/layer_engine.h"

#include "paint/utils.h"

void LayerEngine::init() {
        
    graphics->createRenderPass(layerRenderPass,
                               VK_ATTACHMENT_LOAD_OP_LOAD);

    graphics->createDescriptorSetLayout(layerDescriptorSetLayout);
    graphics->createDescriptorPool(layerDescriptorPool,
                                   MAX_LAYER_COUNT);

    auto layerVertShaderCode = Graphics::readFile(resolveBundlePath("default_layer_vert.spv"));
    auto layerFragShaderCode = Graphics::readFile(resolveBundlePath("default_layer_frag.spv"));

    VkShaderModule layerVertShaderModule = graphics->createShaderModule(layerVertShaderCode);
    VkShaderModule layerFragShaderModule = graphics->createShaderModule(layerFragShaderCode);

    VkPipelineShaderStageCreateInfo layerVertShaderStageInfo{};
    layerVertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    layerVertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    layerVertShaderStageInfo.module = layerVertShaderModule;
    layerVertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo layerFragShaderStageInfo{};
    layerFragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    layerFragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    layerFragShaderStageInfo.module = layerFragShaderModule;
    layerFragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo layerShaderStages[] = { layerVertShaderStageInfo, layerFragShaderStageInfo };

    graphics->createPipeline(layerPipeline,
                             layerDescriptorSetLayout,
                             layerPipelineLayout,
                             layerRenderPass,
                             layerShaderStages);

    graphics->destroyShaderModule(layerVertShaderModule);
    graphics->destroyShaderModule(layerFragShaderModule);
    
}

void LayerEngine::setCanvas(int canvasWidth, int canvasHeight) {
    
    this->canvasWidth = canvasWidth;
    this->canvasHeight = canvasHeight;
    
}

void LayerEngine::createDescriptorSet(VkImageView& imageView, VkDescriptorSet& descriptorSet) {
    
    graphics->createDescriptorSet(imageView,
                                  descriptorSet,
                                  layerDescriptorSetLayout,
                                  layerDescriptorPool);

}

void LayerEngine::setTarget(VkImageView& imageView) {
            
    graphics->createFramebuffer(layerFrameBuffer,
                                layerRenderPass,
                                imageView,
                                canvasWidth,
                                canvasHeight);
    
}

void LayerEngine::recordCommandBuffer(VkCommandBuffer& commandBuffer,
                                      std::vector<VkDescriptorSet>& descriptorSets) {
        
    graphics->recordBeginRenderPass(commandBuffer,
                                    layerRenderPass,
                                    layerFrameBuffer,
                                    canvasWidth,
                                    canvasHeight,
                                    layerPipeline);
    
    graphics->recordSetViewport(commandBuffer,
                                0.0f,
                                0.0f,
                                canvasWidth,
                                canvasHeight);
        
    graphics->recordSetScissor(commandBuffer,
                               0.f,
                               0.f,
                               canvasWidth,
                               canvasHeight);
    
    graphics->recordClearAttachment(commandBuffer, // clear canvas
                                    0.f,
                                    0.f,
                                    canvasWidth,
                                    canvasHeight);
    
    for (VkDescriptorSet& descriptorSet : descriptorSets) {
        
        graphics->recordBindDescriptorSet(commandBuffer,
                                          layerPipelineLayout,
                                          descriptorSet);

        graphics->recordDraw(commandBuffer);
        
    }

    graphics->recordEndRenderPass(commandBuffer);

}

void LayerEngine::recordCommandBuffer(VkCommandBuffer& commandBuffer,
                                      std::vector<glm::ivec2>& tiles,
                                      std::vector<VkDescriptorSet>& descriptorSets) {
        
    graphics->recordBeginRenderPass(commandBuffer,
                                    layerRenderPass,
                                    layerFrameBuffer,
                                    canvasWidth,
                                    canvasHeight,
                                    layerPipeline);
    
    graphics->recordSetViewport(commandBuffer,
                                0.0f,
                                0.0f,
                                canvasWidth,
                                canvasHeight);
    
    for (glm::ivec2 tile : tiles) {
        
        // todo: edge cases aren't right, not drawing at very bottom
            // canvas size: 1024, 2050
        
        if ((canvasWidth - tile.x) <= 0 || (canvasHeight - tile.y) <= 0) continue;
        
        graphics->recordSetScissor(commandBuffer,
                                   tile.x,
                                   tile.y,
                                   CanvasData::TILE_WIDTH,
                                   CanvasData::TILE_HEIGHT);
        
        graphics->recordClearAttachment(commandBuffer, // clear canvas
                                        tile.x,
                                        tile.y,
                                        std::min(CanvasData::TILE_WIDTH, canvasWidth - tile.x),
                                        std::min(CanvasData::TILE_HEIGHT, canvasHeight - tile.y));

        for (VkDescriptorSet& descriptorSet : descriptorSets) {
            
            graphics->recordBindDescriptorSet(commandBuffer,
                                              layerPipelineLayout,
                                              descriptorSet);

            graphics->recordDraw(commandBuffer);
            
        }
        
    }
    
    graphics->recordEndRenderPass(commandBuffer);

}


void LayerEngine::cleanup() {
    
    graphics->destroyPipeline(layerPipeline);
    graphics->destroyPipelineLayout(layerPipelineLayout);
    graphics->destroyRenderPass(layerRenderPass);
    graphics->destroyFrameBuffer(layerFrameBuffer);
    graphics->destroyDescriptorPool(layerDescriptorPool);
    graphics->destroyDescriptorSetLayout(layerDescriptorSetLayout);

}
