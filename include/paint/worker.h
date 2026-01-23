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

struct FrameGraph; // forward declaration
class BrushEngine; // forward declaration

class RenderWorker : public QObject {
    Q_OBJECT
    
public:
    RenderWorker(Graphics* graphics, BrushEngine* brushEngine)
    : graphics(graphics), brushEngine(brushEngine) {}
    
    void processCameraNode(FrameGraph* frameGraph);
    
public slots:
    void onQueueFrame(FrameGraph frameGraph);
    
signals:
    void frameReady(FrameGraph frameGraph);
    
private:
    Graphics* graphics;
    BrushEngine* brushEngine;
    
};
