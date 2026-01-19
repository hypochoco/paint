//
//  worker.cpp
//  main
//
//  Created by Daniel Cho on 1/11/26.
//

#include "paint/worker.h"

#include "paint/brush.h"

RenderWorker::RenderWorker(RenderSystem* renderSystem) {
    this->renderSystem = renderSystem; // todo: remove ?
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
    qDebug() << "[render worker] process brush, raw brush size: "
        << brushStroke->rawBrushPoints.size() << ", submitted index: "
        << brushStroke->submitedIndex;
    
    if (brushStroke->processed) return;
    if (brushStroke->submitedIndex == brushStroke->rawBrushPoints.size() - 1) return;
    
    // todo: move pipelines to brush engine
    
    renderSystem->stamp(graphics->commandBuffers[frameGraph->currentFrame],
                        BrushEngine::interpolate(brushStroke,
                                                 frameGraph->cx, frameGraph->cy, frameGraph->cz,
                                                 frameGraph->windowWidth, frameGraph->windowHeight));
    
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
