//
//  render_worker.cpp
//  main
//
//  Created by Daniel Cho on 1/11/26.
//

#include "paint/render_system/render_worker.h"

#include <chrono>

void RenderWorker::buildBrushStrokeNode(FrameGraph& frameGraph, BrushStrokeData& brushStrokeData) {
        
    // note: without caching 0-3 ms, with caching 0 ms
    
    std::vector<BrushPoint> brushPoints =
        brushEngine->interpolate(frameGraph.camera,
                                 frameGraph.windowSize,
                                 brushStrokeData,
                                 actionDataCache->getBrushStrokeDataCache(brushStrokeData.id));
    
//    brushEngine->calculateTile(brushPoints);
    
    BrushStrokeNode* brushStrokeNode = new BrushStrokeNode { brushPoints }; // todo: make these per tile
    
    LayerNode* layerNode = new LayerNode; // todo: take inputs, handle multiple ways of construction
    layerNode->children.push_back(brushStrokeNode);
        
    frameGraph.root->children.push_back(layerNode);

}

// ---

void RenderWorker::processCameraNode(FrameGraph& frameGraph) {
    
    glm::mat4 view, proj;
    frameGraph.camera.calculateMatrices(view, proj, frameGraph.windowSize);
    graphics->updateGlobalUBO(frameGraph.currentFrame, view, proj);

}

void RenderWorker::processBrushStrokeNode(FrameGraph& frameGraph, BrushStrokeNode& brushStrokeNode) {
    
    brushEngine->stamp(graphics->commandBuffers[frameGraph.currentFrame],
                       brushStrokeNode.brushPoints);
        
}

void RenderWorker::processLayerNode(FrameGraph& frameGraph) {
    
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

// --- 

void RenderWorker::onQueueFrame(FrameGraph frameGraph) {
    qDebug() << "[render worker] start frame: \n" << frameGraph;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    frameGraph.build(*this);
    
    // ---
    
    // todo: move to frame graph building
    
    int selectedLayer = 0; // todo: get this from brush stroke data
    
    brushEngine->setCanvasData(frameGraph.canvasData);
    
    Layer& layer = frameGraph.canvasData.layers[selectedLayer];
    if (frameBufferMap.find(layer.id) == frameBufferMap.end()) {
        VkFramebuffer frameBuffer;
        brushEngine->createFrameBuffer(layer.imageView, frameBuffer);
        frameBufferMap[layer.id] = frameBuffer;
    }
    brushEngine->setTarget(frameBufferMap[layer.id]);
    
    // ---
    
    graphics->beginCommandBuffer(frameGraph.currentFrame);
    
    dfs(frameGraph.root, [&frameGraph, this](Node* node) { node->process(frameGraph, *this); });
    
    graphics->recordSwapChainCommandBuffer(frameGraph.currentFrame);
    graphics->endCommandBuffer(frameGraph.currentFrame);
    
    frameGraph.cleanup();
    
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    
    qDebug() << "[render worker] processing frame took: "
    << duration.count() << " milliseconds";

    emit frameReady(frameGraph);
}

void RenderWorker::cleanup() {
    
    for (auto& it : frameBufferMap) {
        graphics->destroyFrameBuffer(it.second);
    }
    
}
