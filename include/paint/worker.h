//
//  worker.h
//  main
//
//  Created by Daniel Cho on 1/11/26.
//

#pragma once

#include "paint/render.h"

struct RenderJob; // forward declaration
class RenderSystem; // forward declaration
struct BrushStroke; // forward declaration

class RenderWorker : public QObject {
    Q_OBJECT
    
public:
    RenderWorker(RenderSystem* renderSystem);
    
    void updateCamera(float x, float y, float z, float aspect);
    void processBrush(float cx, float cy, float cz,
                      uint32_t width, uint32_t height,
                      BrushStroke brushStroke);
        
public slots:
    void onRender(RenderJob job);
    
signals:
    void queuePresent(RenderJob job);
        
private:
    RenderSystem* renderSystem;
    Graphics* graphics;
    
    RenderJob* renderJob;
};
