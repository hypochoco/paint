//
//  frame_graph.cpp
//  main
//
//  Created by Daniel Cho on 1/8/26.
//

#include "paint/render.h"

#include "paint/utils.h"

// todo: exposed clock into stop and start

RenderSystem::RenderSystem(QVulkanInstance* inst) {
    this->inst = inst;
    
    graphics = new Graphics;
    graphics->setInstance(inst->vkInstance());
    
    renderThread = new QThread;
    renderWorker = new RenderWorker(this);
    
    connect(this, &RenderSystem::queueRender,
            renderWorker, &RenderWorker::onRender);
    
    connect(renderWorker, &RenderWorker::queuePresent,
            this, &RenderSystem::onPresent);
    
    renderWorker->moveToThread(renderThread);
    renderThread->start();

}

RenderSystem::~RenderSystem() {
    renderThread->quit();
    renderThread->wait();
    delete timer;
    delete renderWorker;
    delete graphics;
    delete renderThread;
}

void RenderSystem::initCanvas() {
    
    float aspect = canvasWidth / (float) canvasHeight;

    graphics->createTexture(canvasWidth,
                            canvasHeight,
                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                            | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                            | VK_IMAGE_USAGE_TRANSFER_DST_BIT
                            | VK_IMAGE_USAGE_SAMPLED_BIT,
                            1);

    graphics->addDrawJob(0, 0, 1, std::vector<glm::mat4> { glm::scale(glm::mat4 { 1.0f },
                                                                      glm::vec3 { aspect, 1.0f, 1.0f }) });
    
    for (int i = 0; i< graphics->MAX_FRAMES_IN_FLIGHT; i++) {
        graphics->copyInstanceToBuffer(i); // todo: not a big fan of this update
    }

}

void RenderSystem::initStamp() {
    
    graphics->createRenderPass(stampRenderPass,
                               VK_ATTACHMENT_LOAD_OP_LOAD);

    graphics->createDescriptorSetLayout(stampDescriptorSetLayout);
    graphics->createDescriptorPool(stampDescriptorPool);

    auto stampVertShaderCode = Graphics::readFile(resolveBundlePath("brush_vert.spv"));
    auto stampFragShaderCode = Graphics::readFile(resolveBundlePath("brush_frag.spv"));

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
    pushConstantRange.size = sizeof(StampPushConstant); // pos(vec2) + size(vec2) => 4 floats

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
                                (int) canvasWidth,
                                (int) canvasHeight);

}

void RenderSystem::stamp(VkCommandBuffer& commandBuffer, float x, float y) {
    
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

    StampPushConstant pc { { x, y }, { 0.25f, 0.25f } };

    graphics->recordPushConstant(commandBuffer,
                                 stampPipelineLayout,
                                 sizeof(pc),
                                 &pc);

    graphics->recordDraw(commandBuffer);

    graphics->recordEndRenderPass(commandBuffer);

}

void RenderSystem::initCamera() {
    camera = new Camera;
    camera->z = 3.f;
    cameraDirty.resize(graphics->MAX_FRAMES_IN_FLIGHT, true);
}

void RenderSystem::initFrameGraphs() {
    frameGraphs.resize(graphics->MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < graphics->MAX_FRAMES_IN_FLIGHT; i++) {
        frameGraphs[i] = new FrameGraph;
    }
}

void RenderSystem::init() {
    
    qDebug() << "[render system] init";

    if (!surfaceCreated) {
        throw std::logic_error("[render system] surface not created");
    }

    graphics->pickPhysicalDevice();
    graphics->createLogicalDevice();
    graphics->createCommandPool();
    graphics->createCommandBuffers();
    graphics->createSyncObjects();

    graphics->createTextureSampler();

    graphics->loadQuad();

    graphics->createVertexBuffer();
    graphics->createIndexBuffer();
    graphics->createUniformBuffers();

    initCanvas();
    initStamp();
    
    graphics->createSwapChain();
    graphics->createSwapChainImageViews();
    graphics->createSwapChainRenderPass();
    graphics->createSwapChainDescriptorSetLayout();
    graphics->createSwapChainGraphicsPipeline(resolveBundlePath("vert.spv"),
                                              resolveBundlePath("frag.spv"));
    graphics->createSwapChainColorResources();
    graphics->createSwapChainDepthResources();
    graphics->createSwapChainFramebuffers();
    graphics->createSwapChainDescriptorPool();
    graphics->createSwapChainDescriptorSets();
    
    initCamera();
    initFrameGraphs();

    initialized = true;
    
}

void RenderSystem::recordFrameGraph() {
    
    // note: sending jobs to worker
    
    FrameGraph* frameGraph = currentFrameGraph(); // todo: don't use pointers here ?
    
    if (resized) {
        resized = false;
        cameraDirty.assign(graphics->MAX_FRAMES_IN_FLIGHT, true);
    }
    
    if (cameraDirty[graphics->currentFrame]) {
        frameGraph->addCameraEvent(camera->x, camera->y, camera->z, windowAspect);
        cameraDirty[graphics->currentFrame] = false;
    }
    
    if (brushStroke.has_value()) {
        frameGraph->addBrushEvent(camera->x, camera->y, camera->z,
                                  windowWidth, windowHeight,
                                  brushStroke.value());
        brushStroke.reset();
    }

}

void RenderSystem::render() {
    
    if (!initialized) {
        init();
    }
    
    if (graphics->getSwapChainExtent().width == 0
        || graphics->getSwapChainExtent().height == 0) {
        return;
    }
    
    if (resized) {
        graphics->recreateSwapChain();
    }

    graphics->waitForFences();
    
    emit queryToolSystem();
    
    recordFrameGraph();
    if (currentFrameGraph()->empty()) return;
    
    if (graphics->aquireNextImage()) {
        graphics->recreateSwapChain();
    }
        
    emit queueRender(RenderJob { graphics->imageIndex, graphics->currentFrame });
    
    graphics->advanceFrame(); // note: only for functions in this scope
    
}

void RenderSystem::onPresent(RenderJob job) {
    qDebug() << "[render system] on present";
    graphics->queueSubmit(job.currentFrame);
    if (graphics->queuePresent(job.imageIndex, job.currentFrame)) {
        resized = true;
    }
    frameGraphs[job.currentFrame]->clear();
}

void RenderSystem::cleanup() {
    qDebug() << "[render system] cleanup";
    
    stop(); // note: stop qtimer 
    
    for (FrameGraph* frameGraph : frameGraphs) {
        delete frameGraph;
    }
    
    delete camera;
    
    graphics->deviceWaitIdle();
    
    graphics->destroyPipeline(stampPipeline);
    graphics->destroyPipelineLayout(stampPipelineLayout);
    graphics->destroyRenderPass(stampRenderPass);
    graphics->destroyFrameBuffer(stampFrameBuffer);
    graphics->destroyDescriptorPool(stampDescriptorPool);
    graphics->destroyDescriptorSetLayout(stampDescriptorSetLayout);

    graphics->cleanupVulkan();
}
