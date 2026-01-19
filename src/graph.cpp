//
//  graph.cpp
//  main
//
//  Created by Daniel Cho on 1/11/26.
//

#include "paint/graph.h"

// nodes

void CameraNode::execute(RenderWorker* worker) {
    worker->updateCamera();
}

void BrushNode::execute(RenderWorker* worker) {
    worker->processBrush(brushStroke);
}

// events

void CameraEvent::build(Node* node) {
    node->children.push_back(new CameraNode {});
}

void BrushEvent::build(Node* node) {
    node->children.push_back( new BrushNode { brushStroke });
}
