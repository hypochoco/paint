//
//  action_data_cache.h
//  main
//
//  Created by Daniel Cho on 1/26/26.
//

#pragma once

#include "paint/render_system/cache/brush_stroke_data_cache.h"

struct ActionDataCache {
    
    // todo: clear cache
    
    BrushStrokeDataCache& getBrushStrokeDataCache(uint32_t id) {
        if (id != brushStrokeDataCache.id) {
            brushStrokeDataCache.id = id;
            brushStrokeDataCache.initialized = false;
        }
        return brushStrokeDataCache;
    }
    
private:
    BrushStrokeDataCache brushStrokeDataCache;
    
};
