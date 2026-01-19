//
//  tool.cpp
//  main
//
//  Created by Daniel Cho on 1/12/26.
//

#include "paint/tool.h"

void BrushStroke::addEvent(FrameGraph* frameGraph) {
    frameGraph->addBrushEvent(this);
}
