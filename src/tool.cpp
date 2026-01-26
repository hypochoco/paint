//
//  tool.cpp
//  main
//
//  Created by Daniel Cho on 1/12/26.
//

#include "paint/tool.h"

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

ToolSystem::ToolSystem() {
    actionBuffer = new ActionBuffer<75>;
    tool = new BrushTool { actionBuffer };
}

ToolSystem::~ToolSystem() {
    delete tool;
    delete actionBuffer;
}

void ToolSystem::leftButtonPressed(int x, int y) {
    qDebug() << "[tool system] left button pressed";
    if (tool->leftButtonPressed(x, y)) {
        emit actionsAvailable();
    }
}

void ToolSystem::leftButtonReleased(int x, int y) {
    qDebug() << "[tool system] left button released";
    if (tool->leftButtonReleased(x, y)) {
        emit actionsAvailable();
    }
}

void ToolSystem::mouseMoved(int x, int y) {
    // note: only triggered when left mouse button down
    if (tool->mouseMoved(x, y)) {
        emit actionsAvailable();
    }
}

void ToolSystem::onQueryActions(std::function<void(Action*)> reply) {
    
    if (actionBuffer->size() == 0) return;
        
    for (size_t i = 0, idx = actionBuffer->tail; i < actionBuffer->size(); ++i) {
        Action* action = (*actionBuffer)[(idx + i) % actionBuffer->capacity];
        switch (action->state) {
            case ActionState::RECORDING:
                reply(action);
                action->update();
                break;
            case ActionState::COMPLETE:
                qDebug() << "[tool system] processing action " << i;
                reply(action);
                action->setState(ActionState::PROCESSED);
                break;
            default:
                break;
        }
    }
}
