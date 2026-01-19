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

void RenderWorker::updateCamera() {
    qDebug() << "[render worker] update camera";
    
    glm::mat4 view, proj;
    view = glm::lookAt(glm::vec3(frameGraph->cx, frameGraph->cy, frameGraph->cz), // camera pos
                       glm::vec3(frameGraph->cx, frameGraph->cy, 0.0f), // look at
                       glm::vec3(0.0f, 1.0f, 0.0f)); // up
    proj = glm::perspective(glm::radians(45.0f), // fovy
                            frameGraph->windowAspect,
                            0.1f, // near
                            10.0f); // far
    proj[1][1] *= -1; // strange projection fix
    
    graphics->updateGlobalUBO(frameGraph->currentFrame, view, proj);
}

void RenderWorker::processBrush(BrushStroke* brushStroke) {
    qDebug() << "[render worker] process brush " << brushStroke->rawBrushPoints.size();
    
//    RawBrushPoint rawBrushPoint = brushStroke.rawBrushPoints.back();
//    auto [x, y] = BrushEngine::screenToWorldSpace(cx, cy, cz,
//                                                  width, height,
//                                                  rawBrushPoint.x, rawBrushPoint.y);
//    renderSystem->stamp(graphics->commandBuffers[renderJob->currentFrame], x, y);
    
}

void RenderWorker::traverse(Node* node) {
    if (!node) return;
    for (Node* child : node->children) {
        traverse(child);
    }
    node->execute(this);
}

void RenderWorker::onRender(FrameGraph* frameGraph) {
    this->frameGraph = frameGraph;
    this->frameGraph->build();
        
    graphics->beginCommandBuffer(this->frameGraph->currentFrame);
    traverse(this->frameGraph->root);
    graphics->recordSwapChainCommandBuffer(this->frameGraph->currentFrame);
    graphics->endCommandBuffer(this->frameGraph->currentFrame);
    
    emit queuePresent(this->frameGraph);
}
