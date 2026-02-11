//
//  frame_graph_builder.h
//  paint
//
//  Created by Daniel Cho on 1/11/26.
//

#pragma once

#include "paint/render_system/frame_graph/frame_graph.h"

#include "paint/canvas/canvas_data.h"
#include "paint/tool_system/actions/brush_stroke_data.h"

struct FrameGraphBuilder {
    
    uint32_t imageIndex, currentFrame;
    glm::vec2 windowSize;
    Camera camera;
    CanvasData canvasData;
    
    std::vector<Event*> events;
    
    FrameGraphBuilder& withImageIndex(uint32_t imageIndex) {
        this->imageIndex = imageIndex;
        return *this;
    };
    
    FrameGraphBuilder& withCurrentFrame(uint32_t currentFrame) {
        this->currentFrame = currentFrame;
        return *this;
    };
    
    FrameGraphBuilder& withWindowSize(int width, int height) {
        this->windowSize = glm::vec2{ width, height };
        return *this;
    };
    
    FrameGraphBuilder& withCamera(Camera camera) {
        this->camera = camera;
        return *this;
    };
    
    FrameGraphBuilder& withCanvasData(CanvasData canvasData) {
        this->canvasData = canvasData;
        return *this;
    };
        
    FrameGraphBuilder& addCameraEvent();
    FrameGraphBuilder& addBrushStrokeEvent(BrushStrokeData* brushStrokeData);
    FrameGraphBuilder& addLayerEvent();

    FrameGraph build() {
        return FrameGraph {
            currentFrame,
            imageIndex,
            windowSize,
            camera,
            canvasData,
            events
        };
    }
};
