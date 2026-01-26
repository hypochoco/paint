//
//  worker.cpp
//  main
//
//  Created by Daniel Cho on 1/11/26.
//

#include "paint/worker.h"

void RenderWorker::processCameraNode(FrameGraph& frameGraph) {
    qDebug() << "[render worker] processing camera node";
    
    glm::mat4 view, proj;
    view = glm::lookAt(frameGraph.camera.position, // camera pos
                       glm::vec3(frameGraph.camera.position.x,
                                 frameGraph.camera.position.y,
                                 0.0f), // look at
                       glm::vec3(0.0f, 1.0f, 0.0f)); // up
    proj = glm::perspective(glm::radians(45.0f), // fovy
                            frameGraph.windowSize.x / (float) frameGraph.windowSize.y,
                            0.1f, // near
                            10.0f); // far
    proj[1][1] *= -1; // strange projection fix

    graphics->updateGlobalUBO(frameGraph.currentFrame, view, proj);

}

void RenderWorker::processBrushStrokeNode(FrameGraph& frameGraph, BrushStrokeNode& brushStrokeNode) {
    qDebug() << "[render worker] processing brush stroke node"
    << "\n\traw brush stroke size: " << brushStrokeNode.brushStrokeData.brushPoints.size();
    
    // todo: consider caching brushstroke information in the worker
    
    brushEngine->stamp(graphics->commandBuffers[frameGraph.currentFrame],
                       frameGraph.camera,
                       frameGraph.windowSize,
                       brushStrokeNode.brushStrokeData);
}

void RenderWorker::onQueueFrame(FrameGraph frameGraph) {
    qDebug() << "[render worker] frame graph: \n" << frameGraph;
    
    frameGraph.build();
    
    graphics->beginCommandBuffer(frameGraph.currentFrame);
    
    dfs(frameGraph.root, [&frameGraph, this](Node* node) { node->process(frameGraph, *this); });
    
    graphics->recordSwapChainCommandBuffer(frameGraph.currentFrame);
    graphics->endCommandBuffer(frameGraph.currentFrame);
    
    frameGraph.cleanup();
    
    emit frameReady(frameGraph);
}
