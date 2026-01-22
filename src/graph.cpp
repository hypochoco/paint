//
//  graph.cpp
//  main
//
//  Created by Daniel Cho on 1/22/26.
//

#include "paint/graph.h"

void FrameGraph::build() {
    root = new Node;
    for (Event* event : events) {
        event->build(root);
    }
}

void FrameGraph::cleanup() {
    for (Event* event : events) {
        delete event; // note: action clone delete here
    }
    events.clear();
    dfs(root, [](Node* node) { delete node; });
}

FrameGraphBuilder& FrameGraphBuilder::addCameraEvent() {
    qDebug() << "[frame graph builder] adding camera event";
    events.push_back(new CameraEvent);
    return *this;
};

FrameGraphBuilder& FrameGraphBuilder::addBrushStrokeEvent() {
    // todo: add the brush stroke event
    return *this;
};
