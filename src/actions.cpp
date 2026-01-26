//
//  action.cpp
//  main
//
//  Created by Daniel Cho on 1/24/26.
//

#include "paint/actions.h"

void BrushStroke::addEvent(FrameGraphBuilder& builder) {
    qDebug() << "[brush stroke] event created";
    builder.addBrushStrokeEvent(data.clone());
}
