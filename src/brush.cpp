//
//  brush.cpp
//  main
//
//  Created by Daniel Cho on 1/11/26.
//

#include "paint/brush.h"

std::array<float, 2> BrushEngine::screenToWorldSpace(float cx, float cy, float cz,
                                                     uint32_t width, uint32_t height,
                                                     float x, float y) {
    
    float ndcX = (x / (float) width) * 2.0f - 1.0f;
    float ndcY = 1.0f - (y / (float) height) * 2.0f;

    float aspect = width / (float) height; // window aspect
    float tanHalfFovy = 0.4142f; // hard coded for 45 deg

    return {
        ndcX * cz * tanHalfFovy * aspect + cx,
        ndcY * cz * tanHalfFovy + cy
    };

}

//void BrushEngine::interpolate(BrushStroke brushStroke) {
//    
//    float spacing = 0.5f; // todo: move this elsewhere
//    
//    return;
//}
