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
    dfs(root, [](Node* node) { delete node; });
    for (Event* event : events) {
        delete event; // note: action data clones delete here
    }
    events.clear();
}

FrameGraphBuilder& FrameGraphBuilder::addCameraEvent() {
    qDebug() << "[frame graph builder] adding camera event";
    events.push_back(new CameraEvent);
    return *this;
};

FrameGraphBuilder& FrameGraphBuilder::addBrushStrokeEvent(BrushStrokeData* brushStrokeData) {
    events.push_back(new BrushStrokeEvent { brushStrokeData });
    return *this;
};
