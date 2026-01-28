//
//  brush_stroke_event.cpp
//  main
//
//  Created by Daniel Cho on 1/22/26.
//

#include "paint/render_system/frame_graph/brush_stroke_event.h"

#include "paint/render_system/frame_graph/brush_stroke_node.h"

BrushStrokeEvent::~BrushStrokeEvent() { delete brushStrokeData; }

void BrushStrokeEvent::build(Node* node) {
    // todo: tile calculation here
    node->children.push_back( new BrushStrokeNode { *brushStrokeData });
}
