//
//  brush.cpp
//  main
//
//  Created by Daniel Cho on 1/11/26.
//

#include "paint/brush.h"

glm::vec2 BrushEngine::screenToWorldSpace(float cx, float cy, float cz,
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

std::vector<BrushPoint> BrushEngine::interpolate(BrushStroke* brushStroke,
                                                 float cx, float cy, float cz,
                                                 uint32_t width, uint32_t height) {
    
    // todo: a lot of wasted computation ?
    
    // todo: qt stopping issue is more prevalent, figure out where this is coming from 
    
    float spacing = 0.05f; // temp
    float carry = 0.0f;

    std::vector<BrushPoint> stamps;
    if (brushStroke->rawBrushPoints.empty()) return stamps;

    BrushPoint lastInput = brushStroke->rawBrushPoints[0];
    lastInput.position = screenToWorldSpace(
        cx, cy, cz, width, height,
        lastInput.position.x, lastInput.position.y
    );

    if (brushStroke->submitedIndex == 0) {
        stamps.push_back(lastInput);
    }

    for (size_t i = 1; i < brushStroke->rawBrushPoints.size(); ++i) {

        BrushPoint current = brushStroke->rawBrushPoints[i];
        current.position = screenToWorldSpace(
            cx, cy, cz, width, height,
            current.position.x, current.position.y
        );

        glm::vec2 segment = current.position - lastInput.position;
        float segLen = glm::length(segment);

        if (segLen == 0.0f) {
            lastInput = current;
            continue;
        }

        glm::vec2 dir = segment / segLen;
        float traveled = 0.0f;

        while (traveled + spacing - carry <= segLen) {
            float step = spacing - carry;
            traveled += step;
            if (i > brushStroke->submitedIndex) {
                stamps.push_back(BrushPoint(lastInput.position + dir * traveled));
            }
            carry = 0.0f;
        }

        carry += segLen - traveled;
        lastInput = current;
    }

    return stamps;
}
