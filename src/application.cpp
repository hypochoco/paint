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
    
    // canvas texture
            
    canvasIndex = (uint32_t)graphics.textureImages.size();
    graphics.createTexture(CANVAS_WIDTH,
                           CANVAS_HEIGHT,
                           VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                           | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                           | VK_IMAGE_USAGE_TRANSFER_DST_BIT
                           | VK_IMAGE_USAGE_SAMPLED_BIT,
                           1);

    canvasIndex = (uint32_t)graphics.textureImages.size();
    graphics.createTexture(CANVAS_WIDTH,
                           CANVAS_HEIGHT,
                           VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                           | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                           | VK_IMAGE_USAGE_TRANSFER_DST_BIT
                           | VK_IMAGE_USAGE_SAMPLED_BIT,
                           1);
    
    // brush
    
    brushIndex = (uint32_t)graphics.textureImages.size();
    graphics.loadTexture("brush.png",
                         VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                         | VK_IMAGE_USAGE_TRANSFER_DST_BIT
                         | VK_IMAGE_USAGE_SAMPLED_BIT,
                         1);
            
    // layer
    
    layerIndex = (uint32_t)graphics.textureImages.size();
    graphics.createTexture(CANVAS_WIDTH,
                           CANVAS_HEIGHT,
                           VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                           | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                           | VK_IMAGE_USAGE_TRANSFER_DST_BIT
                           | VK_IMAGE_USAGE_SAMPLED_BIT,
                           1);
        
    // load canvas quad
    
    graphics.loadQuad();
        
    // finish rest of vulkan setup
    
    graphics.initRender("vert.spv", "frag.spv"); // vertex buffers + swap chain pipeline

    // brush pipeline
    
    // brush render pass
    
    graphics.createRenderPass(brushRenderPass,
                              VK_ATTACHMENT_LOAD_OP_LOAD);
    
    // brush descriptors
            
    graphics.createFramebuffer(brushFrameBuffer,
                               brushRenderPass,
                               graphics.textureImageViews[layerIndex],
                               (int) CANVAS_WIDTH,
                               (int) CANVAS_HEIGHT);
    
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
    
    // layer pipeline
    
    // layer render pass
    
    graphics.createRenderPass(layerRenderPass,
                              VK_ATTACHMENT_LOAD_OP_CLEAR);
    
    // layer descriptors
            
    graphics.createFramebuffer(layerFrameBuffer,
                               layerRenderPass,
                               graphics.textureImageViews[canvasIndex],
                               (int) CANVAS_WIDTH,
                               (int) CANVAS_HEIGHT);
    
    graphics.createDescriptorSetLayout(layerDescriptorSetLayout);
    graphics.createDescriptorPool(layerDescriptorPool);
    graphics.createDescriptorSets(graphics.textureImageViews[layerIndex],
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
    
    for (uint32_t i = 0; i < (uint32_t) graphics.MAX_FRAMES_IN_FLIGHT; i++) {
        graphics.updateGlobalUBO(i, view, proj);
    }

    // canvas instances
    
    graphics.materials.emplace_back( Material { 0 } );
    graphics.materials.emplace_back( Material { 1 } );
    
    graphics.addDrawJob(0, 0, 2, std::vector<glm::mat4> {
        glm::mat4(1.0f),
        glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f))
    });
        
    for (uint32_t i = 0; i < (uint32_t) graphics.MAX_FRAMES_IN_FLIGHT; i++) {
        graphics.copyInstanceToBuffer(i);
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
    
    graphics.recordCommandBuffer(commandBuffer,
                                 brushRenderPass,
                                 brushFrameBuffer,
                                 CANVAS_WIDTH,
                                 CANVAS_HEIGHT,
                                 brushPipeline,
                                 CANVAS_WIDTH,
                                 CANVAS_HEIGHT,
                                 CANVAS_WIDTH, // todo: could be optimized
                                 CANVAS_HEIGHT, // todo: could be optimized
                                 brushPipelineLayout,
                                 brushDescriptorSets,
                                 pc);
        
    // layer to canvas

    graphics.recordCommandBuffer(commandBuffer,
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
            if (brushSize > 0.0f) {
                brushSize -= 0.01f;
            }
            inputSystem.reset();
        } else if (inputSystem.rightBracketPressed) {
            brushSize += 0.01f;
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
        
    vkDestroyDescriptorSetLayout(graphics.device, brushDescriptorSetLayout, nullptr);
    vkDestroyDescriptorSetLayout(graphics.device, layerDescriptorSetLayout, nullptr);
    
    graphics.cleanup();

}
