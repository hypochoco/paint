//
//  worker.h
//  main
//
//  Created by Daniel Cho on 1/11/26.
//

#pragma once

#include "paint/render.h"
#include "paint/tool.h"

struct Node; // forward declaration
class RenderSystem; // forward declaration

class RenderWorker : public QObject {
    Q_OBJECT
    
public:
    RenderWorker(RenderSystem* renderSystem);
    
    void updateCamera();
    void processBrush(BrushStroke* brushStroke);
        
public slots:
    void onRender(FrameGraph* frameGraph);
    
signals:
    void queuePresent(FrameGraph* frameGraph);
        
private:
    RenderSystem* renderSystem;
    Graphics* graphics;
    FrameGraph* frameGraph;
    
    void traverse(Node* node);
};
