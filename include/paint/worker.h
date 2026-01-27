//
//  worker.h
//  main
//
//  Created by Daniel Cho on 1/11/26.
//

#pragma once

#include <engine/graphics/graphics.h>

#include "paint/brush.h"
#include "paint/graph.h"
#include "paint/nodes.h"
#include "paint/cache.h"

struct FrameGraph; // forward declaration
class BrushEngine; // forward declaration
struct BrushStrokeNode; // forward declaration

class RenderWorker : public QObject {
    Q_OBJECT
    
public:
    RenderWorker(Graphics* graphics, BrushEngine* brushEngine)
    : graphics(graphics), brushEngine(brushEngine) {
        actionDataCache = new ActionDataCache;
    }
    
    ~RenderWorker() {
        delete actionDataCache;
    }
    
    void processCameraNode(FrameGraph& frameGraph);
    void processBrushStrokeNode(FrameGraph& frameGraph, BrushStrokeNode& brushStrokeNode);
    
public slots:
    void onQueueFrame(FrameGraph frameGraph);
    
signals:
    void frameReady(FrameGraph frameGraph);
    
private:
    Graphics* graphics;
    BrushEngine* brushEngine;
    ActionDataCache* actionDataCache;
    
};
