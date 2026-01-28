//
//  camera_node.cpp
//  main
//
//  Created by Daniel Cho on 1/22/26.
//

#include "paint/render_system/frame_graph/brush_stroke_node.h"

void BrushStrokeNode::process(FrameGraph& frameGraph, RenderWorker& renderWorker) {
    renderWorker.processBrushStrokeNode(frameGraph, *this);
}
