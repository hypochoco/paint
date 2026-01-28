//
//  tool_system.cpp
//  main
//
//  Created by Daniel Cho on 1/12/26.
//

#include "paint/tool_system/tool_system.h"

#include "paint/tool_system/brush_tool.h"

ToolSystem::ToolSystem() {
    actionBuffer = new ActionBuffer<ACTION_BUFFER_SIZE>;
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
