//
//  brush.hpp
//  main
//
//  Created by Daniel Cho on 1/11/26.
//

#pragma once

#include "paint/tool.h"

class BrushEngine {
    
public:
    static std::array<float, 2> screenToWorldSpace(float cx, float cy, float cz,
                                                   uint32_t width, uint32_t height,
                                                   float x, float y);
//    static void interpolate(BrushStroke brushStroke);
    
};
