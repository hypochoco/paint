//
//  camera_event.cpp
//  main
//
//  Created by Daniel Cho on 1/22/26.
//

#include "paint/render_system/frame_graph/camera_event.h"

#include "paint/render_system/frame_graph/camera_node.h"

void CameraEvent::build(FrameGraph& frameGraph, RenderWorker& renderWorker) {
    frameGraph.root->children.push_back(new CameraNode);
}
