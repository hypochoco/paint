//
//  frame_graph_builder.cpp
//  main
//
//  Created by Daniel Cho on 2/1/26.
//

#include "paint/render_system/frame_graph/frame_graph_builder.h"

#include "paint/render_system/frame_graph/camera_event.h"
#include "paint/render_system/frame_graph/brush_stroke_event.h"
#include "paint/render_system/frame_graph/layer_event.h"

FrameGraphBuilder& FrameGraphBuilder::addCameraEvent() {
    events.push_back(new CameraEvent);
    return *this;
};

FrameGraphBuilder& FrameGraphBuilder::addBrushStrokeEvent(BrushStrokeData* brushStrokeData) {
    events.push_back(new BrushStrokeEvent { brushStrokeData });
    return *this;
};

FrameGraphBuilder& FrameGraphBuilder::addLayerEvent() {
    events.push_back(new LayerEvent);
    return *this;
};

