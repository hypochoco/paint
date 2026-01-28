//
//  brush_stroke_event.h
//  main
//
//  Created by Daniel Cho on 1/22/26.
//

#pragma once

#include "paint/render_system/frame_graph/event.h"
#include "paint/tool_system/actions/brush_stroke_data.h"

struct BrushStrokeEvent : public Event {
    BrushStrokeData* brushStrokeData;
    BrushStrokeEvent(BrushStrokeData* brushStrokeData) : brushStrokeData(brushStrokeData) {}
    ~BrushStrokeEvent();
    void build(Node* node) override;
};
