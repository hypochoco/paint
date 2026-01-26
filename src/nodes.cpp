//
//  nodes.cpp
//  main
//
//  Created by Daniel Cho on 1/22/26.
//

#include "paint/nodes.h"

void CameraNode::process(FrameGraph& frameGraph, RenderWorker& renderWorker) {
    renderWorker.processCameraNode(frameGraph);
}

void BrushStrokeNode::process(FrameGraph& frameGraph, RenderWorker& renderWorker) {
    renderWorker.processBrushStrokeNode(frameGraph, *this);
}
