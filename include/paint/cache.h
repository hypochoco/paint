//
//  cache.h
//  main
//
//  Created by Daniel Cho on 1/26/26.
//

#pragma once

#include <cstdint>
#include <cstddef>

#include "paint/brush_point.h"

struct DataCache {
    uint32_t id;
    bool initialized = false;
};

struct BrushStrokeDataCache : public DataCache {
    float carry;
    size_t currentIndex;
    BrushPoint lastInput;
};

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
