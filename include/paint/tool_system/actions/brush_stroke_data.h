//
//  actions.h
//  main
//
//  Created by Daniel Cho on 1/21/26.
//

#pragma once

#include <vector>

#include "paint/tool_system/actions/action_data.h"
#include "paint/tool_system/actions/brush_point.h"

struct BrushStrokeData : public ActionData {
    
    int nextIndex = 0;
    std::vector<BrushPoint> brushPoints;
    
    BrushStrokeData* clone() override {
        return new BrushStrokeData(*this);
    }
};
