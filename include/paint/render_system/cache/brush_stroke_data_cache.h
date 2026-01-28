//
//  brush_stroke_data_cache.h
//  main
//
//  Created by Daniel Cho on 1/26/26.
//

#pragma once

#include <cstddef>

#include "paint/render_system/cache/data_cache.h"
#include "paint/tool_system/actions/brush_point.h"

struct BrushStrokeDataCache : public DataCache {
    float carry;
    size_t currentIndex;
    BrushPoint lastInput;
};
