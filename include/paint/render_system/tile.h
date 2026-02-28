//
//  tile.h
//  main
//
//  Created by Daniel Cho on 2/23/26.
//

#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "paint/tool_system/actions/brush_point.h"

struct Tile {
    
    glm::ivec4 dimensions;
    std::vector<BrushPoint> brushPoints;
    
    // debug
    
    glm::vec4 debugColor;
    
};
