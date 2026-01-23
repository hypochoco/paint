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
#include <algorithm>

#include <engine/graphics/graphics.h>

#include "paint/actions.h"
#include "paint/graph.h"
#include "paint/worker.h"
#include "paint/brush.h"
#include "paint/camera.h"

struct Action; // forward declaration
struct FrameGraph; // forward declaration
class RenderWorker; // forward declaration

enum DirtyFlag {
    RESIZED,
    CAMERA,
    ACTIONS
};

class DirtyFlags {
    
public:
    void init(uint32_t maxFramesInFlight) {
        camera.resize(maxFramesInFlight);
    }

    void set(DirtyFlag flag) {
        switch (flag) {
            case DirtyFlag::RESIZED:
                qDebug() << "[dirty flags] set resized flag";
                resized = true;
                camera.assign(camera.size(), true);
                break;
            case DirtyFlag::ACTIONS:
                // qDebug() << "[dirty flags] set actions flag";
                actions = true;
                break;
            default:
                qDebug() << "[dirty flags] warning: flag not implemented";
                break;
        }
    }
    
    void clear(int frame) {
        resized = false;
        camera[frame] = false;
        actions = false;
    }
    
    bool dirty() {
        return resized
        || std::any_of(camera.begin(), camera.end(), [](bool b){ return b; })
        || actions;
    }
    
    bool dirty(DirtyFlag flag) {
        switch (flag) {
            case DirtyFlag::RESIZED:
                return resized;
            case DirtyFlag::ACTIONS:
                qDebug() << "[dirty flags] actions flag: " << actions;
                return actions;
            default:
                qDebug() << "[dirty flags] warning: flag not implemented";
                return false;
        }
    }
    
    bool dirty(DirtyFlag flag, int currentFrame) {
        switch (flag) {
            case DirtyFlag::CAMERA:
                qDebug() << "[dirty flags] camera flag: " << camera[currentFrame];
                return camera[currentFrame];
            default:
                qDebug() << "[dirty flags] warning: flag not implemented";
                return false;
        }
    }
    
private:
    bool resized;
    std::vector<bool> camera;
    bool actions;

};

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
