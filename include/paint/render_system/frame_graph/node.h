//
//  node.h
//  main
//
//  Created by Daniel Cho on 1/22/26.
//

#pragma once

#include <vector>

class RenderWorker;
struct FrameGraph;

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
