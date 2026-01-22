//
//  nodes.h
//  main
//
//  Created by Daniel Cho on 1/22/26.
//

#pragma once

#include <vector>

#include "paint/graph.h"
#include "paint/worker.h"

struct FrameGraph; // forward declaration
class RenderWorker; // forward declaration

struct Node {
    virtual ~Node() = default;
    std::vector<Node*> children;
    virtual void process(FrameGraph* frameGraph, RenderWorker* renderWorker) {}
};

template <typename F>
inline void dfs(Node* node, F&& op) {
    if (!node) return;
    for (Node* child : node->children) {
        dfs(child, std::forward<F>(op));
    }
    op(node);
}

struct CameraNode : public Node {
    ~CameraNode() override {
        qDebug() << "[camera node] destructor called";
    }
    void process(FrameGraph* frameGraph, RenderWorker* renderWorker) override;
};

//struct BrushNode : public Node {
//    BrushStroke* brushStroke;
//    BrushNode(BrushStroke* brushStroke) : brushStroke(brushStroke) {}
//    void execute(RenderWorker* worker) override;
//    void cleanup() override;
//};

//void BrushNode::execute(RenderWorker* worker) {
//    worker->processBrush(brushStroke);
//}

//void BrushNode::cleanup() {
//    delete brushStroke;
//}
