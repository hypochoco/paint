//
//  application.cpp
//  engine
//
//  Created by Daniel Cho on 9/30/25.
//

#include "paint/application.h"

#include <stdexcept>
#include <iostream>

// paint application

Application::Application() { // constructor
    // currently empty
}

Application::~Application() { // destructor
    // cleanup here
}

void Application::init() {
    
    graphics.initWindow(); // todo: configurables here ? window size
    graphics.initVulkan();
        
    // brush
    
    graphics.loadTexture("brush.png",
                         brushTextureImage,
                         brushTextureImageMemory,
                         brushTextureImageView,
                         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                         | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                         | VK_IMAGE_USAGE_TRANSFER_DST_BIT
                         | VK_IMAGE_USAGE_SAMPLED_BIT,
                         1);
    
    graphics.transitionImageLayout(brushTextureImage,
                                   VK_FORMAT_R8G8B8A8_SRGB,
                                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                   1);
        
    // layer
    
    graphics.createTexture(CANVAS_WIDTH,
                           CANVAS_HEIGHT,
                           layerTextureImage,
                           layerTextureImageMemory,
                           layerTextureImageView,
                           VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                           | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                           | VK_IMAGE_USAGE_TRANSFER_DST_BIT
                           | VK_IMAGE_USAGE_SAMPLED_BIT,
                           1);
    
    graphics.transitionImageLayout(layerTextureImage,
                                   VK_FORMAT_R8G8B8A8_SRGB,
                                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   1);
    
    // canvas texture
            
    graphics.createTexture(CANVAS_WIDTH,
                           CANVAS_HEIGHT,
                           canvasTextureImage,
                           canvasTextureImageMemory,
                           canvasTextureImageView,
                           VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                           | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                           | VK_IMAGE_USAGE_TRANSFER_DST_BIT
                           | VK_IMAGE_USAGE_SAMPLED_BIT,
                           1);
    
    graphics.transitionImageLayout(canvasTextureImage,
                                   VK_FORMAT_R8G8B8A8_SRGB,
                                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                   1);
    
    graphics.textureImages.push_back(canvasTextureImage);
    graphics.textureImageMemories.push_back(canvasTextureImageMemory);
    graphics.textureImageViews.push_back(canvasTextureImageView);
    
    // load canvas quad
    
    auto canvasQuad = Graphics::loadCanvasQuad();
    graphics.pushModel(canvasQuad); // todo: which objs to draw, with which materials
    
    // finish rest of vulkan setup
    
    graphics.initRender("vert.spv", "frag.spv"); // vertex buffers + swap chain pipeline

    // brush pipeline
    
    // brush render pass
    
    graphics.createRenderPass(brushRenderPass,
                              VK_ATTACHMENT_LOAD_OP_LOAD);
    
    // brush descriptors
            
    graphics.createFramebuffer(brushFrameBuffer,
                               brushRenderPass,
                               layerTextureImageView,
                               (int) CANVAS_WIDTH,
                               (int) CANVAS_HEIGHT);
    
    graphics.createDescriptorSetLayout(brushDescriptorSetLayout);
    
    graphics.createDescriptorPool(brushDescriptorPool);
    
    graphics.createDescriptorSets(brushTextureImageView,
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
    
    // layer pipeline
    
    // layer render pass
    
    graphics.createRenderPass(layerRenderPass,
                              VK_ATTACHMENT_LOAD_OP_CLEAR);
    
    // layer descriptors
            
    graphics.createFramebuffer(layerFrameBuffer,
                               layerRenderPass,
                               canvasTextureImageView,
                               (int) CANVAS_WIDTH,
                               (int) CANVAS_HEIGHT);
    
    graphics.createDescriptorSetLayout(layerDescriptorSetLayout);
    
    graphics.createDescriptorPool(layerDescriptorPool);
    
    graphics.createDescriptorSets(layerTextureImageView,
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
    
    // default camera
    
    int windowWidth, windowHeight;
    glfwGetWindowSize(graphics.window, &windowWidth, &windowHeight);
    
    depth = 3.0f;
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, depth), // camera pos
                                 glm::vec3(0.0f, 0.0f, 0.0f), // look at
                                 glm::vec3(0.0f, 1.0f, 0.0f)); // up
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), // fovy
                                      windowWidth / (float) windowHeight, // aspect
                                      0.1f, // near
                                      10.0f); // far
    proj[1][1] *= -1; // strange projection fix

    std::vector<InstanceSSBO> instances(1);
    instances[0].model = glm::mat4(1.0f);
    
    for (uint32_t i = 0; i < (uint32_t) graphics.MAX_FRAMES_IN_FLIGHT; i++) {
        graphics.updateGlobalUBO(i, view, proj);
        graphics.updateInstanceSSBOs(i, instances);
    }
    
}

void Application::draw() {
    
    uint32_t imageIndex;
    
    graphics.startFrame(imageIndex);
    
    // other globalubo + instancessbo updates here
    
    VkCommandBuffer& commandBuffer = graphics.commandBuffers[graphics.currentFrame];
    
    // brush to layer
    
    // screen space to world space
    
    int windowWidth, windowHeight;
    glfwGetWindowSize(graphics.window, &windowWidth, &windowHeight);
    
    float ndcX = (inputSystem.xpos / windowWidth) * 2.0f - 1.0f;
    float ndcY = 1.0f - (inputSystem.ypos / windowHeight) * 2.0f;
    
    float aspect = (float) windowWidth / windowHeight;
    float tanHalfFovy = 0.4142f; // hard coded for 45 deg
    
    struct BrushPC { float pos[2]; float size[2]; } pc;
    pc.pos[0] = ndcX * depth * tanHalfFovy * aspect;
    pc.pos[1] = ndcY * depth * tanHalfFovy;
    
    pc.size[0] = brushSize; // translate to px size
    pc.size[1] = brushSize;
    
    graphics.draw(commandBuffer,
                  brushRenderPass,
                  brushFrameBuffer,
                  CANVAS_WIDTH,
                  CANVAS_HEIGHT,
                  brushPipeline,
                  CANVAS_WIDTH,
                  CANVAS_HEIGHT,
                  CANVAS_WIDTH, // could be optimized
                  CANVAS_HEIGHT, // could be optimized
                  brushPipelineLayout,
                  brushDescriptorSets,
                  pc);
        
    // layer to canvas
    
    // todo: minimize the number of image transitions
    
    graphics.transitionImageLayout(commandBuffer,
                                   layerTextureImage,
                                   VK_FORMAT_R8G8B8A8_SRGB,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                   1);
    
    graphics.transitionImageLayout(commandBuffer,
                                   canvasTextureImage,
                                   VK_FORMAT_R8G8B8A8_SRGB,
                                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   1);

    graphics.draw(commandBuffer,
                  layerRenderPass,
                  layerFrameBuffer,
                  CANVAS_WIDTH,
                  CANVAS_HEIGHT,
                  layerPipeline,
                  CANVAS_WIDTH,
                  CANVAS_HEIGHT,
                  CANVAS_WIDTH,
                  CANVAS_HEIGHT,
                  layerPipelineLayout,
                  layerDescriptorSets);
    
    graphics.transitionImageLayout(commandBuffer,
                                   layerTextureImage,
                                   VK_FORMAT_R8G8B8A8_SRGB,
                                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   1);

    graphics.transitionImageLayout(commandBuffer,
                                   canvasTextureImage,
                                   VK_FORMAT_R8G8B8A8_SRGB,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                   1);
    
    // canvas to swapchain
    
    graphics.submitFrame(imageIndex);
    
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    InputSystem* inputSystem = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
    if (!inputSystem) return;
    inputSystem->mouse_button_callback(button, action, mods);
}

void mouse_position_callback(GLFWwindow* window, double xpos, double ypos) {
    InputSystem* inputSystem = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
    if (!inputSystem) return;
    inputSystem->mouse_position_callback(xpos, ypos);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    InputSystem* inputSystem = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
    if (!inputSystem) return;
    inputSystem->key_callback(key, action);
}

void Application::run() {
    
    // register inputs
        
    glfwSetWindowUserPointer(graphics.window, &inputSystem);
    
    glfwSetMouseButtonCallback(graphics.window, mouse_button_callback);
    glfwSetCursorPosCallback(graphics.window, mouse_position_callback);
    glfwSetKeyCallback(graphics.window, key_callback);

    // main loop
    
    draw();
    inputSystem.reset();
    
    // random paint stuff
    brushSize = 0.25f;
    
    // actual main loop
    while (!glfwWindowShouldClose(graphics.window)) {
        glfwPollEvents();
        if (inputSystem.pressed) { // todo: better architecture
            draw();
            inputSystem.reset();
        } else if (inputSystem.leftBracketPressed) {
            brushSize += 0.01f;
            inputSystem.reset();
        } else if (inputSystem.rightBracketPressed) {
            if (brushSize > 0.0f) {
                brushSize -= 0.01f;
            }
            inputSystem.reset();
        }
    }
    vkDeviceWaitIdle(graphics.device);
    
    // todo: add logging for average draw call timing
        // start time
        // count
        // end time / count
    
}

void Application::cleanup() {
    
    vkDestroyPipeline(graphics.device, brushPipeline, nullptr);
    vkDestroyPipelineLayout(graphics.device, brushPipelineLayout, nullptr);
    vkDestroyRenderPass(graphics.device, brushRenderPass, nullptr);
    
    vkDestroyFramebuffer(graphics.device, brushFrameBuffer, nullptr);
    
    vkDestroyPipeline(graphics.device, layerPipeline, nullptr);
    vkDestroyPipelineLayout(graphics.device, layerPipelineLayout, nullptr);
    vkDestroyRenderPass(graphics.device, layerRenderPass, nullptr);
    
    vkDestroyFramebuffer(graphics.device, layerFrameBuffer, nullptr);
    
    vkDestroyDescriptorPool(graphics.device, brushDescriptorPool, nullptr);
    vkDestroyDescriptorPool(graphics.device, layerDescriptorPool, nullptr);
    
    vkDestroyImageView(graphics.device, brushTextureImageView, nullptr);
    vkDestroyImage(graphics.device, brushTextureImage, nullptr);
    vkFreeMemory(graphics.device, brushTextureImageMemory, nullptr);

    vkDestroyImageView(graphics.device, layerTextureImageView, nullptr);
    vkDestroyImage(graphics.device, layerTextureImage, nullptr);
    vkFreeMemory(graphics.device, layerTextureImageMemory, nullptr);
    
    vkDestroyDescriptorSetLayout(graphics.device, brushDescriptorSetLayout, nullptr);
    vkDestroyDescriptorSetLayout(graphics.device, layerDescriptorSetLayout, nullptr);
    
    graphics.cleanup();

}
