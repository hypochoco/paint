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
    
    brushEngine->setCanvasData(frameGraph.canvasData);
    
    // todo: throw an error if selected layer is not visible
    // todo: optimize by grouping everything under the same layer if all drawn
    
    Layer& selectedLayer = frameGraph.canvasData.layers[brushStrokeData.selectedLayer];
    if (frameBufferMap.find(selectedLayer.id) == frameBufferMap.end()) {
        VkFramebuffer frameBuffer;
        brushEngine->createFrameBuffer(selectedLayer.imageView, frameBuffer);
        frameBufferMap[selectedLayer.id] = frameBuffer;
    }
    brushEngine->setTarget(frameBufferMap[selectedLayer.id]);
    
    std::vector<BrushPoint> brushPoints =
        brushEngine->interpolate(frameGraph.camera,
                                 frameGraph.windowSize,
                                 brushStrokeData,
                                 actionDataCache->getBrushStrokeDataCache(brushStrokeData.id));
    
    if (brushPoints.size() == 0) return;
    
    auto tileMap = brushEngine->calculateTile(brushPoints);
    
    std::vector<glm::ivec2> tiles;
    std::unordered_map<glm::ivec2, std::vector<BrushPoint>> canvasMap;
    
    for (auto& it : tileMap) {
        
        glm::ivec2 start = brushEngine->tileToCanvas(it.first);
        tiles.push_back(start);
        canvasMap[start] = it.second;
        
    }
    
    BrushStrokeNode* brushStrokeNode = new BrushStrokeNode { canvasMap };
    
    LayerNode* layerNode = new LayerNode { tiles };
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
    
    brushEngine->recordCommandBuffer(graphics->commandBuffers[frameGraph.currentFrame],
                                     brushStrokeNode.canvasMap);
        
}

void RenderWorker::processLayerNode(FrameGraph& frameGraph, LayerNode& layerNode) {
    
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
    
    if (layerNode.tiles.size() != 0) {
        layerEngine->recordCommandBuffer(graphics->commandBuffers[frameGraph.currentFrame],
                                         layerNode.tiles,
                                         descriptorSets);
    } else {
        layerEngine->recordCommandBuffer(graphics->commandBuffers[frameGraph.currentFrame],
                                         descriptorSets);
    }
    
}

// --- 

void RenderWorker::onQueueFrame(FrameGraph frameGraph) {
    qDebug() << "[render worker] start frame: \n" << frameGraph;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    frameGraph.build(*this);
    
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
