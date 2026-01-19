//
//  graph.cpp
//  main
//
//  Created by Daniel Cho on 1/11/26.
//

#include "paint/graph.h"

// nodes

void CameraNode::execute(RenderWorker* worker) {
    worker->updateCamera(x, y, z, aspect);
}

void BrushNode::execute(RenderWorker *worker) {
    worker->processBrush(x, y, z, width, height, brushStroke);
}

// events

void CameraEvent::build(Node* node) {
    node->children.push_back(new CameraNode { x, y, z, aspect });
}

void BrushEvent::build(Node* node) {
    node->children.push_back( new BrushNode { x, y, z, width, height, brushStroke });
}
