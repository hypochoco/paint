//
//  layer_engine.cpp
//  main
//
//  Created by Daniel Cho on 1/30/26.
//

#include "paint/layer_engine/layer_engine.h"

#include "paint/utils.h"

void LayerEngine::init() {
    
    // default pass
        
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
    
    // debugging pass
    
    auto debuggingLayerVertShaderCode = Graphics::readFile(resolveBundlePath("flat_layer_vert.spv"));
    auto debuggingLayerFragShaderCode = Graphics::readFile(resolveBundlePath("flat_layer_frag.spv"));

    VkShaderModule debuggingLayerVertShaderModule = graphics->createShaderModule(debuggingLayerVertShaderCode);
    VkShaderModule debuggingLayerFragShaderModule = graphics->createShaderModule(debuggingLayerFragShaderCode);

    VkPipelineShaderStageCreateInfo debuggingLayerVertShaderStageInfo{};
    debuggingLayerVertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    debuggingLayerVertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    debuggingLayerVertShaderStageInfo.module = debuggingLayerVertShaderModule;
    debuggingLayerVertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo debuggingLayerFragShaderStageInfo{};
    debuggingLayerFragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    debuggingLayerFragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    debuggingLayerFragShaderStageInfo.module = debuggingLayerFragShaderModule;
    debuggingLayerFragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo debuggingLayerShaderStages[] = {
        debuggingLayerVertShaderStageInfo,
        debuggingLayerFragShaderStageInfo
    };
    
    graphics->createPipeline(debuggingLayerPipeline,
                             layerDescriptorSetLayout,
                             debuggingLayerPipelineLayout,
                             layerRenderPass,
                             debuggingLayerShaderStages);

    graphics->destroyShaderModule(debuggingLayerVertShaderModule);
    graphics->destroyShaderModule(debuggingLayerFragShaderModule);
    
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
    
    qDebug() << "[layer engine] record command buffer";
        
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
                                      std::vector<glm::ivec4>& tiles,
                                      std::vector<VkDescriptorSet>& descriptorSets) {
    
    qDebug() << "[layer engine] tile record command buffer";
        
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
    
    for (glm::ivec4 tile : tiles) { // clear dirty tiles
        // note: separate to portect against overlap flickers
        graphics->recordSetScissor(commandBuffer, tile);
        graphics->recordClearAttachment(commandBuffer, tile);
    }
    
    for (VkDescriptorSet& descriptorSet : descriptorSets) {
        // note: draw by layer, then by tile
        graphics->recordBindDescriptorSet(commandBuffer,
                                          layerPipelineLayout,
                                          descriptorSet);
        for (glm::ivec4 tile : tiles) {
            graphics->recordSetScissor(commandBuffer, tile);
            graphics->recordDraw(commandBuffer);
        }
    }
    
    graphics->recordEndRenderPass(commandBuffer);

}

void LayerEngine::cleanup() {
    
    // debugging
    
    graphics->destroyPipeline(debuggingLayerPipeline);
    graphics->destroyPipelineLayout(debuggingLayerPipelineLayout);
    
    graphics->destroyPipeline(layerPipeline);
    graphics->destroyPipelineLayout(layerPipelineLayout);
    graphics->destroyRenderPass(layerRenderPass);
    graphics->destroyFrameBuffer(layerFrameBuffer);
    graphics->destroyDescriptorPool(layerDescriptorPool);
    graphics->destroyDescriptorSetLayout(layerDescriptorSetLayout);

}
