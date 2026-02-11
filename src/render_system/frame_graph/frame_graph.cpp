//
//  frame_graph.cpp
//  main
//
//  Created by Daniel Cho on 1/22/26.
//

#include "paint/render_system/frame_graph/frame_graph.h"

void FrameGraph::build(RenderWorker& renderWorker) {
    root = new Node;
    for (Event* event : events) {
        event->build(*this, renderWorker);
    }
}

void FrameGraph::cleanup() {
    dfs(root, [](Node* node) { delete node; });
    for (Event* event : events) {
        delete event; // note: action data clones delete here
    }
    events.clear();
}
