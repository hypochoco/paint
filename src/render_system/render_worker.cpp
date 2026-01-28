//
//  render_worker.cpp
//  main
//
//  Created by Daniel Cho on 1/11/26.
//

#include "paint/render_system/render_worker.h"

#include <chrono>

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
    << "\n\tid: " << brushStrokeNode.brushStrokeData.id
    << "\n\traw brush stroke size: " << brushStrokeNode.brushStrokeData.brushPoints.size();
    
    // note: without caching 0-3 ms, with caching 0 ms
    
    auto start = std::chrono::high_resolution_clock::now();
        
    brushEngine->stamp(graphics->commandBuffers[frameGraph.currentFrame],
                       frameGraph.camera,
                       frameGraph.windowSize,
                       brushStrokeNode.brushStrokeData,
                       actionDataCache->getBrushStrokeDataCache(brushStrokeNode.brushStrokeData.id));
    
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    
    qDebug() << "[render worker] processing brush stroke node took: "
    << duration.count() << " milliseconds";
    
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
