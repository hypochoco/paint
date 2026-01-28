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

#include "paint/brush_engine/brush_engine.h"
#include "paint/render_system/render_worker.h"
#include "paint/render_system/dirty_flags.h"
#include "paint/tool_system/actions/action.h"

class RenderSystem : public QObject {
    Q_OBJECT
    
public:
    RenderSystem(QVulkanInstance* inst);
    ~RenderSystem();
    
    void start() {
        qDebug() << "[render system] started";
        connect(&timer, &QTimer::timeout,
                this, &RenderSystem::update);
        // note: 16ms interval (~60fps)
        timer.start(32); // note: 32 for testing
    }
    
    void stop() {
        qDebug() << "[render system] stopped";
        timer.stop();
        timer.disconnect();
    }
        
public slots:
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
    void onSurfaceAbobutToBeDestroyed() {
        qDebug() << "[render system] surface about to be destroyed";
        cleanup();
    }
    
    void onActionsAvailable() {
        dirtyFlags.set(DirtyFlag::ACTIONS);
    }

    void onFrameReady(FrameGraph frameGraph) {
        submitFrame(frameGraph);
    }
    
private slots:
    void update();
    
signals:
    void queryActions(std::function<void(Action*)> reply);
    
    void queueFrame(FrameGraph frameGraph);
    void requestUpdate();
    
private:
    QTimer timer;
    
    QVulkanInstance* inst;
    Graphics* graphics;
    BrushEngine* brushEngine;
    QThread* renderThread;
    RenderWorker* renderWorker;
    
    bool surfaceCreated;
    bool exposed;
    bool initialized;
    
    int windowWidth, windowHeight;
    
    uint32_t canvasWidth = 1024; // todo: canvas data struct
    uint32_t canvasHeight = 1024;
    
    Camera camera;
    DirtyFlags dirtyFlags;
        
    void initCanvas();
    void init();
        
    void startFrame();
    void submitFrame(FrameGraph frameGraph);
    
    void cleanup();
    
};
