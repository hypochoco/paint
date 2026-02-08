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
    frameGraph.camera.calculateMatrices(view, proj, frameGraph.windowSize);
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
    
    processLayerNode(frameGraph); // todo: temp
        
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    
    qDebug() << "[render worker] processing brush stroke node took: "
    << duration.count() << " milliseconds";
    
}

void RenderWorker::processLayerNode(FrameGraph& frameGraph) {
    qDebug() << "[render worker] processing layer node";
    
    // todo: tiling
    // todo: layer visibility
    
    std::vector<VkDescriptorSet> descriptorSets;
    for (int i = 0; i < frameGraph.canvasData.layers.size(); i++) {
        Layer& layer = frameGraph.canvasData.layers[i];
        if (descriptorSetMap.find(layer.id) == descriptorSetMap.end()) { // todo: move to build
            VkDescriptorSet descriptorSet;
            layerEngine->createDescriptorSet(layer.imageView, descriptorSet);
            descriptorSetMap[layer.id] = descriptorSet;
        }
        descriptorSets.push_back(descriptorSetMap[layer.id]);
    }
    
    layerEngine->stamp(graphics->commandBuffers[frameGraph.currentFrame],
                       descriptorSets);
}

void RenderWorker::onQueueFrame(FrameGraph frameGraph) {
    qDebug() << "[render worker] frame graph: \n" << frameGraph;
    
    frameGraph.build(); // todo
    
    // ---
    
    // todo: move to frame graph building
    
    brushEngine->setCanvasData(frameGraph.canvasData);
    
    Layer& layer = frameGraph.canvasData.layers[frameGraph.selectedLayer];
    if (frameBufferMap.find(layer.id) == frameBufferMap.end()) {
        VkFramebuffer frameBuffer;
        brushEngine->createFrameBuffer(layer.imageView, frameBuffer);
        frameBufferMap[layer.id] = frameBuffer;
    }
    brushEngine->setTarget(frameBufferMap[layer.id]); // todo: should be part of brushstroke data
    
    // ---
    
    graphics->beginCommandBuffer(frameGraph.currentFrame);
    
    dfs(frameGraph.root, [&frameGraph, this](Node* node) { node->process(frameGraph, *this); });
    
    graphics->recordSwapChainCommandBuffer(frameGraph.currentFrame);
    graphics->endCommandBuffer(frameGraph.currentFrame);
    
    frameGraph.cleanup();
    
    emit frameReady(frameGraph); // todo: error when adding the canvas ?
}

void RenderWorker::cleanup() {
    
    for (auto& it : frameBufferMap) {
        graphics->destroyFrameBuffer(it.second);
    }
    
}
