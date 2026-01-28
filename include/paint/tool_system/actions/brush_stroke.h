//
//  actions.h
//  main
//
//  Created by Daniel Cho on 1/21/26.
//

#pragma once

#include "paint/tool_system/actions/action.h"
#include "paint/tool_system/actions/brush_point.h"
#include "paint/tool_system/actions/brush_stroke_data.h"

struct BrushStroke : public Action {
    
    BrushStrokeData data;
            
    void record(int x, int y) override {
        data.brushPoints.emplace_back(BrushPoint { glm::vec2(x,y) });
    }
    
    void update() override { // note: called after each action query
        data.nextIndex = (int) data.brushPoints.size();
    }
    
    void addEvent(FrameGraphBuilder& builder) override;
    
};
