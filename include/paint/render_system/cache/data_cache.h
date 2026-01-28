//
//  data_cache.h
//  main
//
//  Created by Daniel Cho on 1/26/26.
//

#pragma once

#include <cstdint>

#include "paint/tool_system/actions/brush_point.h"

struct DataCache {
    uint32_t id;
    bool initialized = false;
};
