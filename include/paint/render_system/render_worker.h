//
//  worker.h
//  main
//
//  Created by Daniel Cho on 1/11/26.
//

#pragma once

#include <QObject>

#include <engine/graphics/graphics.h>

#include "paint/brush_engine/brush_engine.h"
#include "paint/render_system/cache/action_data_cache.h"
#include "paint/render_system/frame_graph/frame_graph.h"
#include "paint/render_system/frame_graph/brush_stroke_node.h"

struct FrameGraph; // forward delcaration
struct BrushStrokeNode; // forward delcaration

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
