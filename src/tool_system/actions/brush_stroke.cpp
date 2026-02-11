//
//  brush_stroke.cpp
//  main
//
//  Created by Daniel Cho on 1/24/26.
//

#include "paint/tool_system/actions/brush_stroke.h"

void BrushStroke::addEvent(FrameGraphBuilder& builder) {
    builder.addBrushStrokeEvent(data.clone());
}
