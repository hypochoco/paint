//
//  brush_tool.cpp
//  main
//
//  Created by Daniel Cho on 1/12/26.
//

#include "paint/tool_system/brush_tool.h"

#include "paint/tool_system/actions/brush_stroke.h"

bool BrushTool::leftButtonPressed(int x, int y) {
    qDebug() << "[brush tool] left button pressed";
    Action* action = new BrushStroke;
    action->record(x, y);
    actionBuffer->push(action);
    return true;
}

bool BrushTool::leftButtonReleased(int x, int y) {
    qDebug() << "[brush tool] left button released";
    actionBuffer->front()->record(x, y);
    actionBuffer->front()->setState(ActionState::COMPLETE);
    return true;
}

bool BrushTool::mouseMoved(int x, int y) {
    // note: only triggered when left mouse button down
    actionBuffer->front()->record(x, y);
    return true;
}
