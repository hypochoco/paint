//
//  camera_node.cpp
//  main
//
//  Created by Daniel Cho on 1/22/26.
//

#include "paint/render_system/frame_graph/camera_node.h"

void CameraNode::process(FrameGraph& frameGraph, RenderWorker& renderWorker) {
    renderWorker.processCameraNode(frameGraph);
}
