//
//  render_system.cpp
//  main
//
//  Created by Daniel Cho on 1/8/26.
//

#include "paint/render_system/render_system.h"

#include "paint/utils.h"

RenderSystem::RenderSystem(QVulkanInstance* inst) : inst(inst) {
    
    graphics = new Graphics;
    graphics->setInstance(inst->vkInstance());
    
    dirtyFlags.init(graphics->MAX_FRAMES_IN_FLIGHT);
    
    brushEngine = new BrushEngine(graphics);
    
    renderThread = new QThread;
    renderWorker = new RenderWorker(graphics,
                                    brushEngine);
    
    connect(this, &RenderSystem::queueFrame,
            renderWorker, &RenderWorker::onQueueFrame,
            Qt::QueuedConnection);
    
    connect(renderWorker, &RenderWorker::frameReady,
            this, &RenderSystem::onFrameReady,
            Qt::QueuedConnection);
    
    renderWorker->moveToThread(renderThread);
    renderThread->start();
}

RenderSystem::~RenderSystem() {
    delete renderWorker;
    delete renderThread;
    delete brushEngine;
    delete graphics;
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

    graphics->addDrawJob(0, 0, 1,
                         std::vector<glm::mat4> { glm::scale(glm::mat4 { 1.0f },
                                                             glm::vec3 { aspect, 1.0f, 1.0f }) });
    
    for (int i = 0; i< graphics->MAX_FRAMES_IN_FLIGHT; i++) {
        graphics->copyInstanceToBuffer(i); // todo: change graphics api
    }

}

void RenderSystem::init() {
    
    qDebug() << "[render system] init";

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

    graphics->createSwapChain();
    graphics->createSwapChainImageViews();
    graphics->createSwapChainRenderPass();
    graphics->createSwapChainDescriptorSetLayout();
    graphics->createSwapChainGraphicsPipeline(resolveBundlePath("shader_vert.spv"),
                                              resolveBundlePath("shader_frag.spv"));
    graphics->createSwapChainColorResources();
    graphics->createSwapChainDepthResources();
    graphics->createSwapChainFramebuffers();
    graphics->createSwapChainDescriptorPool();
    
    initCanvas();
    graphics->createSwapChainDescriptorSets();
    camera.position.z = 3.f; // note: default camera position
    brushEngine->init();

    initialized = true;
    
}

void RenderSystem::update() {
    
    // ensure init and conditions
    
    if (!surfaceCreated) return;
    if (!exposed) return;
    if (!initialized) init();
    if (graphics->getSwapChainExtent().width == 0
        || graphics->getSwapChainExtent().height == 0) {
        return;
    }
    if (!dirtyFlags.dirty()) return;
    
    // start frame if all conditions pass
    
    startFrame();
    
}

void RenderSystem::startFrame() {
    
    if (dirtyFlags.dirty(DirtyFlag::RESIZED)) {
        graphics->recreateSwapChain();
    }

    graphics->waitForFences();
    
    if (!graphics->aquireNextImage()) {
        graphics->recreateSwapChain();
    }
    
    qDebug() << "[render system] start frame\n\timage index: "
    << graphics->imageIndex << "\n\tcurrent frame: "
    << graphics->currentFrame;
    
    FrameGraphBuilder builder;
    builder.withImageIndex(graphics->imageIndex)
        .withCurrentFrame(graphics->currentFrame)
        .withCamera(camera)
        .withWindowSize(windowWidth, windowHeight);
    
    if (dirtyFlags.dirty(DirtyFlag::CAMERA, graphics->currentFrame)) {
        builder.addCameraEvent();
    }
    
    if (dirtyFlags.dirty(DirtyFlag::ACTIONS)) {
        emit queryActions([&builder](Action* action) {
            action->addEvent(builder);
        });
    }
    
    dirtyFlags.clear(graphics->currentFrame);
    
    emit queueFrame(builder.build());
    
    graphics->advanceFrame();
    
}

void RenderSystem::submitFrame(FrameGraph frameGraph) {
    
    qDebug() << "[render system] submit frame";
    
    graphics->queueSubmit(frameGraph.currentFrame);
    if (!graphics->queuePresent(frameGraph.imageIndex,
                                frameGraph.currentFrame)) {
        dirtyFlags.set(DirtyFlag::RESIZED);
    }
    emit requestUpdate(); // note: request update from window
    
}

void RenderSystem::cleanup() {
    
    qDebug() << "[render system] cleanup";
    
    stop(); // note: stop qtimer
    
    renderThread->quit();
    renderThread->wait();
        
    graphics->deviceWaitIdle();
    brushEngine->cleanup();
    graphics->cleanupVulkan();
    
}
