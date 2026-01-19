//
//  frame_graph.h
//  main
//
//  Created by Daniel Cho on 1/8/26.
//

#pragma once

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QTimer>

#include <QVulkanInstance>

#include <iostream>
#include <stdexcept>

#include <engine/graphics/graphics.h>

#include "paint/worker.h"
#include "paint/graph.h"
#include "paint/tool.h"

struct BrushStroke; // forward declaration
struct FrameGraph; // forward declaration

struct RenderJob {
    uint32_t imageIndex;
    uint32_t currentFrame;
};

struct Camera {
    float x, y, z;
};

struct StampPushConstant {
    float pos[2];
    float size[2];
};

class RenderSystem : public QObject {
    Q_OBJECT
    
public:
    RenderSystem(QVulkanInstance* inst);
    ~RenderSystem();
    
    Graphics* graphics;
    Camera* camera;
    std::vector<FrameGraph*> frameGraphs;
    
    void start() {
        qDebug() << "[render system] started";
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout,
                this, &RenderSystem::render,
                Qt::QueuedConnection);
        timer->start(16); // 16ms interval (~60fps)
    }
    
    void stop() {
        qDebug() << "[render system] stopped";
        timer->stop();
        timer->disconnect(this);
    }
    
    void init();
    void render();
    void cleanup();
    
    void updateCamera(uint32_t currentFrame);
    std::array<float, 2> screenToWorldSpace(double xpos, double ypos);
    void stamp(VkCommandBuffer& commandBuffer, float x, float y);
    
public slots:
    void onSurfaceCreated(QWindow* window) {
        qDebug() << "[render system] surface set";
        graphics->setSurface(QVulkanInstance::surfaceForWindow(window));
        surfaceCreated = true;
    }
    void onExposed(bool isExposed) { // todo
        if (isExposed) {
            start();
        } else {
            stop();
        }
    }
    void onResized(uint32_t width, uint32_t height) {
        qDebug() << "[render system] surface resized";
        this->windowWidth = width;
        this->windowHeight = height;
        this->windowAspect = width / (float) height;
        resized = true;
    }
    void onPresent(RenderJob job);
    void onSurfaceAbobutToBeDestroyed() {
        qDebug() << "[render system] surface about to be destroyed";
        cleanup();
    }
    void onBrushStroke(BrushStroke brushStroke) {
        qDebug() << "[render system] on brush stroke";
        this->brushStroke = brushStroke;
    }
    
signals:
    void queueRender(RenderJob job);
    void queryToolSystem();
        
private:
    QVulkanInstance* inst;
    QThread* renderThread;
    RenderWorker* renderWorker;
    
    QTimer* timer;
    
    // todo: canvas data struct 
    
    bool surfaceCreated;
    bool initialized;
    
    uint32_t windowWidth, windowHeight;
    float windowAspect;
    bool resized;
    std::vector<bool> cameraDirty;
    
    std::optional<BrushStroke> brushStroke;
    
    uint32_t canvasWidth = 1024;
    uint32_t canvasHeight = 1024;
    
    void initCanvas();
    void initStamp();
    void initCamera();
    void initFrameGraphs();
    FrameGraph* currentFrameGraph() {
        return frameGraphs[graphics->currentFrame];
    }
    void recordFrameGraph();
    
    VkRenderPass stampRenderPass;
    VkFramebuffer stampFrameBuffer;
    VkDescriptorSetLayout stampDescriptorSetLayout;
    VkDescriptorPool stampDescriptorPool;
    std::vector<VkDescriptorSet> stampDescriptorSets;
    VkPipeline stampPipeline;
    VkPipelineLayout stampPipelineLayout;

};
