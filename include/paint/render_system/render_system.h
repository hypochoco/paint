//
//  render_system.h
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
#include <algorithm>

#include <engine/graphics/graphics.h>

#include "paint/render_system/dirty_flags.h"
#include "paint/brush_engine/brush_engine.h"
#include "paint/layer_engine/layer_engine.h"
#include "paint/render_system/render_worker.h"
#include "paint/tool_system/actions/action.h"
#include "paint/canvas/canvas_data.h"

class RenderSystem : public QObject {
    Q_OBJECT
    
public:
    RenderSystem(Graphics* graphics);
    ~RenderSystem();
        
    void setCanvasData(CanvasData* canvasData) {
        this->canvasData = canvasData;
    }

    void cleanup();
        
public slots:
    void onStart() {
        start();
    }
    void onSurfaceCreated(QWindow* window) {
        qDebug() << "[render system] surface set";
        graphics->setSurface(QVulkanInstance::surfaceForWindow(window));
        surfaceCreated = true;
    }
    void onResized(int width, int height) {
        qDebug() << "[render system] surface resized";
        dirtyFlags.set(DirtyFlag::RESIZED);
        windowWidth = width;
        windowHeight = height;
    }
    void onExposed(bool isExposed) {
        exposed = isExposed;
    }
    
    void onLayersDirty() {
        dirtyFlags.set(DirtyFlag::LAYER);
    }
    
    void onActionsAvailable() {
        dirtyFlags.set(DirtyFlag::ACTIONS);
    }
    void onFrameReady(FrameGraph frameGraph) {
        submitFrame(frameGraph);
    }
    
    void onCleanup() {
        cleanup();
    }
    
private slots:
    void update();
    
signals:
    void ready();
    void querySelectedLayer(std::function<void(int)> reply);
    void queryActions(std::function<void(Action*)> reply);
    void queueFrame(FrameGraph frameGraph);
    void requestUpdate();
    
private:
    QTimer timer;
    
    Graphics* graphics;
    BrushEngine* brushEngine;
    LayerEngine* layerEngine;
    QThread* renderThread;
    RenderWorker* renderWorker;
    
    bool surfaceCreated;
    bool exposed;
    bool initialized;
    bool canvasInitialized;
    
    int windowWidth, windowHeight;
    
    CanvasData* canvasData;
            
    Camera camera;
    DirtyFlags dirtyFlags;
    
    void start() {
        qDebug() << "[render system] started";
        connect(&timer, &QTimer::timeout,
                this, &RenderSystem::update);
        renderWorker->moveToThread(renderThread);
        renderThread->start();
        // note: 16ms interval (~60fps)
        timer.start(32); // note: 32 for testing
    }
    
    void stop() {
        qDebug() << "[render system] stopped";
        timer.stop();
        timer.disconnect();
    }
        
    void initCanvas();
    void init();
        
    void startFrame();
    void submitFrame(FrameGraph frameGraph);
    
};
