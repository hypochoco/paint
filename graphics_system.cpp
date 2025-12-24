//
//  graphics_system.cpp
//  paint
//
//  Created by Daniel Cho on 12/23/25.
//

#include "paint/graphics_system.h"

void GraphicsSystem::initCanvas() {
    
    // temp: constant size
    
    const int canvasWidth = 1024;
    const int canvasHeight = 1024;
    
    // tiling
    
    numRows = std::ceil(canvasHeight / TILE_HEIGHT);
    numCols = std::ceil(canvasWidth / TILE_WIDTH);
    
    firstCanvas = (uint32_t) graphics.textureImages.size();
    canvasCount = numCols * numRows;
    
    modelMatrices.reserve(canvasCount);
    
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            
            graphics.createTexture(TILE_WIDTH,
                                   TILE_HEIGHT,
                                   VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                                   | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                                   | VK_IMAGE_USAGE_TRANSFER_DST_BIT
                                   | VK_IMAGE_USAGE_SAMPLED_BIT,
                                   1);
            
            graphics.materials.emplace_back( Material { (uint32_t) i * numCols + j } );
            
            modelMatrices.emplace_back( glm::translate(glm::mat4(1.0f),
                                                       glm::vec3(2.0f * i, 2.0f * j, 0.0f)) );
            
        }
        
    }
        
}

void GraphicsSystem::initLayer() { // single layer, could be multiple layers
    
    firstLayer = (uint32_t) graphics.textureImages.size();
    layerCount = numCols * numRows;
    
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            
            graphics.createTexture(TILE_WIDTH,
                                   TILE_HEIGHT,
                                   VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                                   | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                                   | VK_IMAGE_USAGE_TRANSFER_DST_BIT
                                   | VK_IMAGE_USAGE_SAMPLED_BIT,
                                   1);
                        
        }
        
    }
    
}

void GraphicsSystem::initBrushPipeline() {
        
    // brush render pass
    
    graphics.createRenderPass(brushRenderPass,
                              VK_ATTACHMENT_LOAD_OP_LOAD);
    
    // brush descriptors
            
    for (int i = 0; i < layerCount; i ++) {
        VkFramebuffer brushFrameBuffer;
        graphics.createFramebuffer(brushFrameBuffer,
                                   brushRenderPass,
                                   graphics.textureImageViews[firstLayer + i],
                                   (int) TILE_WIDTH,
                                   (int) TILE_HEIGHT);
        brushFrameBuffers.push_back(brushFrameBuffer);
    }
    
    graphics.createDescriptorSetLayout(brushDescriptorSetLayout);
    graphics.createDescriptorPool(brushDescriptorPool);
    
    graphics.createDescriptorSets(graphics.textureImageViews[brushIndex],
                                  brushDescriptorSets,
                                  brushDescriptorSetLayout,
                                  brushDescriptorPool);
    
    // brush graphics pipeline
    
    auto brushVertShaderCode = Graphics::readFile("brush_vert.spv");
    auto brushFragShaderCode = Graphics::readFile("brush_frag.spv");
    
    VkShaderModule brushVertShaderModule = graphics.createShaderModule(brushVertShaderCode);
    VkShaderModule brushFragShaderModule = graphics.createShaderModule(brushFragShaderCode);

    VkPipelineShaderStageCreateInfo brushVertShaderStageInfo{};
    brushVertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    brushVertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    brushVertShaderStageInfo.module = brushVertShaderModule;
    brushVertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo brushFragShaderStageInfo{};
    brushFragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    brushFragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    brushFragShaderStageInfo.module = brushFragShaderModule;
    brushFragShaderStageInfo.pName = "main";
    
    VkPipelineShaderStageCreateInfo brushShaderStages[] = { brushVertShaderStageInfo, brushFragShaderStageInfo };
    
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(float) * 4; // pos(vec2) + size(vec2) => 4 floats
    
    graphics.createPipeline(brushPipeline,
                            brushDescriptorSetLayout,
                            brushPipelineLayout,
                            brushRenderPass,
                            brushShaderStages,
                            pushConstantRange);
    
    vkDestroyShaderModule(graphics.device, brushVertShaderModule, nullptr);
    vkDestroyShaderModule(graphics.device, brushFragShaderModule, nullptr);
    
}

void GraphicsSystem::initLayerPipeline() {
        
    // layer render pass
    
    graphics.createRenderPass(layerRenderPass,
                              VK_ATTACHMENT_LOAD_OP_CLEAR);
    
    // layer descriptors
    
    for (int i = 0; i < canvasCount; i++) {
        VkFramebuffer layerFrameBuffer;
        graphics.createFramebuffer(layerFrameBuffer,
                                   layerRenderPass,
                                   graphics.textureImageViews[firstCanvas + i],
                                   (int) TILE_WIDTH,
                                   (int) TILE_HEIGHT);
        layerFrameBuffers.push_back(layerFrameBuffer);
    }
    
    graphics.createDescriptorSetLayout(layerDescriptorSetLayout);
    graphics.createDescriptorPool(layerDescriptorPool);
    graphics.createDescriptorSets(graphics.textureImageViews[firstLayer],
                                  layerDescriptorSets,
                                  layerDescriptorSetLayout,
                                  layerDescriptorPool);
    
    // layer graphics pipeline
    
    auto layerVertShaderCode = Graphics::readFile("layer_vert.spv");
    auto layerFragShaderCode = Graphics::readFile("layer_frag.spv");
    
    VkShaderModule layerVertShaderModule = graphics.createShaderModule(layerVertShaderCode);
    VkShaderModule layerFragShaderModule = graphics.createShaderModule(layerFragShaderCode);

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
    
    graphics.createPipeline(layerPipeline,
                            layerDescriptorSetLayout,
                            layerPipelineLayout,
                            layerRenderPass,
                            layerShaderStages);
    
    vkDestroyShaderModule(graphics.device, layerVertShaderModule, nullptr);
    vkDestroyShaderModule(graphics.device, layerFragShaderModule, nullptr);

}

void GraphicsSystem::initCamera() {
    
    // todo: optimize this for updates, maybe some kind of dirty flags per frame in flight
    
    int windowWidth, windowHeight;
    glfwGetWindowSize(graphics.window, &windowWidth, &windowHeight);
    
    glm::mat4 view = glm::lookAt(glm::vec3(cameraX, cameraY, depth), // camera pos
                                 glm::vec3(cameraX, cameraY, 0.0f), // look at
                                 glm::vec3(0.0f, 1.0f, 0.0f)); // up
    
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), // fovy
                                      windowWidth / (float) windowHeight, // aspect
                                      0.1f, // near
                                      10.0f); // far
    proj[1][1] *= -1; // strange projection fix
        
    for (uint32_t i = 0; i < (uint32_t) graphics.MAX_FRAMES_IN_FLIGHT; i++) {
        graphics.updateGlobalUBO(i, view, proj);
    }
    
}

void GraphicsSystem::init() {
    
    graphics.initWindow(); // todo: window configurables
    
    graphics.initVulkan();
    
    // textures
    
    initCanvas();
    
    initLayer();
            
    brushIndex = (uint32_t) graphics.textureImages.size();
    
    graphics.loadTexture("brush.png",
                         VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                         | VK_IMAGE_USAGE_TRANSFER_DST_BIT
                         | VK_IMAGE_USAGE_SAMPLED_BIT,
                         1);

    // load canvas quad
    
    graphics.loadQuad();
        
    // finish rest of vulkan setup
    
    graphics.initRender("vert.spv", "frag.spv"); // vertex buffers + swap chain pipeline

    // pipelines
    
    initBrushPipeline();
    
    initLayerPipeline();
    
    // default camera
    
    initCamera();
    
    // canvas draw job
    
    graphics.addDrawJob(0, 0, canvasCount, modelMatrices);
    for (uint32_t i = 0; i < (uint32_t) graphics.MAX_FRAMES_IN_FLIGHT; i++) {
        graphics.copyInstanceToBuffer(i);
    }

}

std::array<float, 2> GraphicsSystem::screenToWorldSpace(double xpos, double ypos) {

    // return the worldspace coordinates
    
    // todo: take into account camera position
    
    int windowWidth, windowHeight;
    glfwGetWindowSize(graphics.window, &windowWidth, &windowHeight);

    float ndcX = (xpos / windowWidth) * 2.0f - 1.0f;
    float ndcY = 1.0f - (ypos / windowHeight) * 2.0f;

    float aspect = (float) windowWidth / windowHeight;
    float tanHalfFovy = 0.4142f; // hard coded for 45 deg
    
    return {
        ndcX * depth * tanHalfFovy * aspect + cameraX,
        ndcY * depth * tanHalfFovy + cameraY
    };

}

std::array<int, 2> quadCollision(float xworld, float yworld) {
    
    // from world position, return index of hit quad
    
    // note: assumes each quad is -1 -> 1
    
    return {
        static_cast<int>(std::round(xworld / 2.0)), // col number
        static_cast<int>(std::round(yworld / 2.0)) // row number
    };
    
}

void GraphicsSystem::bufferBrush(double xpos, // screen pos
                                 double ypos, // screen pos
                                 float brushSize) {
    
    // queue brush job
    
    auto [ xworld, yworld ] = screenToWorldSpace(xpos, ypos);
    auto [ i, j ] = quadCollision(xworld, yworld);
    int tile = i * numCols + j;
    
    brushBufferTile.push_back(tile);
    brushBufferXTile.push_back(xworld - i * 2);
    brushBufferYTile.push_back(yworld - j * 2);
    brushBufferBrushSize.push_back(brushSize);

}

void GraphicsSystem::recordBrushCommandBuffer(VkCommandBuffer commandBuffer,
                                              uint32_t tile,
                                              float xtile,
                                              float ytile,
                                              float brushSize) {
    
    // command buffer for brush to tile
    
    struct BrushPC { float pos[2]; float size[2]; } pc;
    pc.pos[0] = xtile; pc.pos[1] = ytile;
    pc.size[0] = brushSize; pc.size[1] = brushSize;
    
    graphics.recordCommandBuffer(commandBuffer,
                                 brushRenderPass,
                                 brushFrameBuffers[tile],
                                 TILE_WIDTH,
                                 TILE_HEIGHT,
                                 brushPipeline,
                                 TILE_WIDTH,
                                 TILE_HEIGHT,
                                 TILE_WIDTH, // todo: could be optimized
                                 TILE_HEIGHT, // todo: could be optimized
                                 brushPipelineLayout,
                                 brushDescriptorSets,
                                 pc);
    
    // update layer to canvas texture target
    
    graphics.updateDescriptorSet(graphics.textureImageViews[firstLayer + tile],
                                 layerDescriptorSets[graphics.currentFrame],
                                 layerDescriptorSetLayout,
                                 layerDescriptorPool);
    
    // layer to canvas
    
    graphics.recordCommandBuffer(commandBuffer,
                                 layerRenderPass,
                                 layerFrameBuffers[tile],
                                 TILE_WIDTH,
                                 TILE_HEIGHT,
                                 layerPipeline,
                                 TILE_WIDTH,
                                 TILE_HEIGHT,
                                 TILE_WIDTH,
                                 TILE_HEIGHT,
                                 layerPipelineLayout,
                                 layerDescriptorSets);

}

void GraphicsSystem::draw() {
        
    uint32_t imageIndex;
    
    graphics.startFrame(imageIndex);
    
    // todo: other globalubo + instancessbo updates here, i.e. panning
            
    // start command buffer
    
    VkCommandBuffer& commandBuffer = graphics.commandBuffers[graphics.currentFrame];
    
    // brush queue
    
    for (int i = 0; i < brushBufferTile.size(); i++) {
        
        uint32_t& tile = brushBufferTile[i];
        
        if (tile < 0 || tile >= canvasCount) continue; // out of bounds
        
        recordBrushCommandBuffer(commandBuffer,
                                 tile,
                                 brushBufferXTile[i],
                                 brushBufferYTile[i],
                                 brushBufferBrushSize[i]);
        
    }
    
    brushBufferTile.clear();
    brushBufferXTile.clear();
    brushBufferYTile.clear();
    brushBufferBrushSize.clear();
    
    // canvas to swapchain
    
    graphics.submitFrame(imageIndex);
    
}

void GraphicsSystem::cleanup() {
    
    vkDestroyPipeline(graphics.device, brushPipeline, nullptr);
    vkDestroyPipelineLayout(graphics.device, brushPipelineLayout, nullptr);
    vkDestroyRenderPass(graphics.device, brushRenderPass, nullptr);
    
    for (auto& brushFrameBuffer : brushFrameBuffers) {
        vkDestroyFramebuffer(graphics.device, brushFrameBuffer, nullptr);
    }
    
    vkDestroyPipeline(graphics.device, layerPipeline, nullptr);
    vkDestroyPipelineLayout(graphics.device, layerPipelineLayout, nullptr);
    vkDestroyRenderPass(graphics.device, layerRenderPass, nullptr);
    
    for (auto& layerFrameBuffer : layerFrameBuffers) {
        vkDestroyFramebuffer(graphics.device, layerFrameBuffer, nullptr);
    }

    vkDestroyDescriptorPool(graphics.device, brushDescriptorPool, nullptr);
    vkDestroyDescriptorPool(graphics.device, layerDescriptorPool, nullptr);
        
    vkDestroyDescriptorSetLayout(graphics.device, brushDescriptorSetLayout, nullptr);
    vkDestroyDescriptorSetLayout(graphics.device, layerDescriptorSetLayout, nullptr);
    
    graphics.cleanup();
    
}
