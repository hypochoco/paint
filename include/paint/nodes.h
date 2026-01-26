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
#include "paint/actions.h"

struct FrameGraph; // forward declaration
class RenderWorker; // forward declaration
struct BrushStrokeData;

struct Node {
    virtual ~Node() = default;
    std::vector<Node*> children;
    virtual void process(FrameGraph& frameGraph, RenderWorker& renderWorker) {}
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
    void process(FrameGraph& frameGraph, RenderWorker& renderWorker) override;
};

struct BrushStrokeNode : public Node {
    BrushStrokeData& brushStrokeData;
    BrushStrokeNode(BrushStrokeData& brushStrokeData) : brushStrokeData(brushStrokeData) {}
    void process(FrameGraph& frameGraph, RenderWorker& renderWorker) override;
};
