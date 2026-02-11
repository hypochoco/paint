//
//  render_system.cpp
//  main
//
//  Created by Daniel Cho on 1/8/26.
//

#include "paint/render_system/render_system.h"

#include "paint/utils.h"

RenderSystem::RenderSystem(Graphics* graphics) : graphics(graphics) {
    
    dirtyFlags.init(graphics->MAX_FRAMES_IN_FLIGHT);
    
    layerEngine = new LayerEngine(graphics);
    brushEngine = new BrushEngine(graphics);
    
    renderThread = new QThread;
    renderWorker = new RenderWorker(graphics,
                                    layerEngine,
                                    brushEngine);
    
    connect(this, &RenderSystem::queueFrame,
            renderWorker, &RenderWorker::onQueueFrame,
            Qt::QueuedConnection);
    
    connect(renderWorker, &RenderWorker::frameReady,
            this, &RenderSystem::onFrameReady,
            Qt::QueuedConnection);
    
}

RenderSystem::~RenderSystem() {
    
    delete renderWorker;
    delete renderThread;
    delete brushEngine;
    delete layerEngine;
    
}

void RenderSystem::init() {
    
    if (initialized) return;
    
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
    
    camera.position.z = 3.f; // note: default camera position
    
    layerEngine->init();
    brushEngine->init();

    initialized = true;
    
    emit ready();
    
}

void RenderSystem::initCanvas() {
    
    if (canvasInitialized) return;
    if (!canvasData) return;
    
    qDebug() << "[render system] canvas init";
    
    float aspect = canvasData->width / (float) canvasData->height;

    graphics->createTexture(canvasData->width, // swapchain texture
                            canvasData->height,
                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                            | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                            | VK_IMAGE_USAGE_TRANSFER_DST_BIT
                            | VK_IMAGE_USAGE_SAMPLED_BIT,
                            1,
                            0);

    graphics->addDrawJob(0, 0, 1,
                         std::vector<glm::mat4> { glm::scale(glm::mat4 { 1.0f },
                                                             glm::vec3 { aspect, 1.0f, 1.0f }) });
    
    for (int i = 0; i< graphics->MAX_FRAMES_IN_FLIGHT; i++) {
        graphics->copyInstanceToBuffer(i); // todo: change graphics api
    }
    
    graphics->createSwapChainDescriptorSets();
    
    layerEngine->setCanvas(canvasData->width, canvasData->height); // todo: move
    layerEngine->setTarget(graphics->textureImageViews[0]);
                    
    dirtyFlags.set(DirtyFlag::LAYER);

    canvasInitialized = true;

}

void RenderSystem::update() {
        
    if (!surfaceCreated) return;
    if (!exposed) return;
    if (!initialized) {
        init();
        if (!initialized) return;
    }
    if (!canvasInitialized) {
        initCanvas();
        if (!canvasInitialized) return;
    }
    if (graphics->getSwapChainExtent().width == 0
        || graphics->getSwapChainExtent().height == 0) {
        return;
    }
    if (!dirtyFlags.dirty()) return;
    
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
    
    FrameGraphBuilder builder; // data copies
    builder.withImageIndex(graphics->imageIndex)
        .withCurrentFrame(graphics->currentFrame)
        .withCamera(camera)
        .withCanvasData(*canvasData)
        .withWindowSize(windowWidth, windowHeight);
        
    if (dirtyFlags.dirty(DirtyFlag::CAMERA, graphics->currentFrame)) {
        builder.addCameraEvent();
    }
    
    if (dirtyFlags.dirty(DirtyFlag::ACTIONS)) {
        emit queryActions([&builder](Action* action) {
            action->addEvent(builder);
        });
    }
    
    if (dirtyFlags.dirty(DirtyFlag::LAYER)) {
        builder.addLayerEvent();
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
        
    brushEngine->cleanup();
    layerEngine->cleanup();
    
    renderWorker->cleanup();
    
}
