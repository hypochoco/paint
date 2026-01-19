//
//  worker.cpp
//  main
//
//  Created by Daniel Cho on 1/11/26.
//

#include "paint/worker.h"

#include "paint/brush.h"

RenderWorker::RenderWorker(RenderSystem* renderSystem) {
    this->renderSystem = renderSystem;
    this->graphics = renderSystem->graphics;
}

void RenderWorker::updateCamera(float x, float y, float z, float aspect) {
    qDebug() << "[render worker] update camera";
    
    glm::mat4 view, proj;
    view = glm::lookAt(glm::vec3(x, y, z), // camera pos
                       glm::vec3(x, y, 0.0f), // look at
                       glm::vec3(0.0f, 1.0f, 0.0f)); // up
    proj = glm::perspective(glm::radians(45.0f), // fovy
                            aspect,
                            0.1f, // near
                            10.0f); // far
    proj[1][1] *= -1; // strange projection fix
    
    graphics->updateGlobalUBO(renderJob->currentFrame, view, proj);
}

void RenderWorker::processBrush(float cx, float cy, float cz,
                                uint32_t width, uint32_t height,
                                BrushStroke brushStroke) {
    qDebug() << "[render worker] process brush " << brushStroke.rawBrushPoints.size();
    
    RawBrushPoint rawBrushPoint = brushStroke.rawBrushPoints.back();
    auto [x, y] = BrushEngine::screenToWorldSpace(cx, cy, cz,
                                                  width, height,
                                                  rawBrushPoint.x, rawBrushPoint.y);
    renderSystem->stamp(graphics->commandBuffers[renderJob->currentFrame], x, y);
}

void RenderWorker::onRender(RenderJob job) {
    qDebug() << "[render worker] on render";
    
    FrameGraph* frameGraph = renderSystem->frameGraphs[job.currentFrame];
    frameGraph->build();
    
    renderJob = &job; // cache render job
    
    graphics->beginCommandBuffer(job.currentFrame);
    for (Node* node : frameGraph->nodes) {
        node->execute(this);
    }
    graphics->recordSwapChainCommandBuffer(job.currentFrame);
    graphics->endCommandBuffer(job.currentFrame);
    
    emit queuePresent(job);
}
