//
//  worker.cpp
//  main
//
//  Created by Daniel Cho on 1/11/26.
//

#include "paint/worker.h"

void RenderWorker::processCameraNode(FrameGraph* frameGraph) {
    qDebug() << "[render worker] processing camera node";
    
    glm::mat4 view, proj;
    view = glm::lookAt(frameGraph->camera.position, // camera pos
                       glm::vec3(frameGraph->camera.position.x,
                                 frameGraph->camera.position.y,
                                 0.0f), // look at
                       glm::vec3(0.0f, 1.0f, 0.0f)); // up
    proj = glm::perspective(glm::radians(45.0f), // fovy
                            frameGraph->windowWidth / (float) frameGraph->windowHeight,
                            0.1f, // near
                            10.0f); // far
    proj[1][1] *= -1; // strange projection fix

    graphics->updateGlobalUBO(frameGraph->currentFrame, view, proj);

}

void RenderWorker::onQueueFrame(FrameGraph frameGraph) {
    qDebug() << "[render worker] frame graph: \n" << frameGraph;
    
    frameGraph.build();
    
    graphics->beginCommandBuffer(frameGraph.currentFrame);
    
    dfs(frameGraph.root, [&frameGraph, this](Node* node) { node->process(&frameGraph, this); });
    
    graphics->recordSwapChainCommandBuffer(frameGraph.currentFrame);
    graphics->endCommandBuffer(frameGraph.currentFrame);
    
    frameGraph.cleanup();
    
    // todo: consider caching brushstroke information in the worker ?
    
    emit frameReady(frameGraph);
}

//void RenderWorker::processBrush(BrushStroke* brushStroke) {
//    qDebug() << "[render worker] process brush, raw brush size: "
//        << brushStroke->rawBrushPoints.size() << ", submitted index: "
//        << brushStroke->submitedIndex;
//    
//    if (brushStroke->processed) return;
//    if (brushStroke->submitedIndex == brushStroke->rawBrushPoints.size() - 1) return;
//    
//    // todo: move pipelines to brush engine
//    
//    renderSystem->stamp(graphics->commandBuffers[frameGraph->currentFrame],
//                        BrushEngine::interpolate(brushStroke,
//                                                 frameGraph->cx, frameGraph->cy, frameGraph->cz,
//                                                 frameGraph->windowWidth, frameGraph->windowHeight));
//    
//}

//void RenderWorker::traverse(Node* node) { // todo: rename as dfs, doesn't need to be in the class ?
//    if (!node) return;
//    for (Node* child : node->children) {
//        traverse(child);
//    }
//    node->execute(this);
//}
