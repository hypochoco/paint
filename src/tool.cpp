//
//  tool.cpp
//  main
//
//  Created by Daniel Cho on 1/12/26.
//

#include "paint/tool.h"

// todo: action safety
// todo: okay if this class + actions is a little messy for

// todo: get functionality back

// todo: persistent subclass state system with transitions ...
    // current state
    // transitions in and out ...
    // ensures all cases are handled well
    // no more switch cases ...

    // actionbuffer is still the central buffer though ...

ToolSystem::ToolSystem() : selectedTool(Tool::BRUSH) {
    actionBuffer = new ActionBuffer<75>;
}

ToolSystem::~ToolSystem() {
    // todo: delete everything in the action buffer
    delete actionBuffer;
}

void ToolSystem::leftButtonPressed(int x, int y) {
    qDebug() << "[tool system] left button pressed";
    Action* action;
    switch (selectedTool) {
        case Tool::BRUSH:
            action = new BrushStroke;
            action->record(x, y);
            break;
        default:
            break;
    }
    if (action) {
        actionBuffer->push(action);
        emit actionsAvailable();
    }
}

void ToolSystem::leftButtonReleased(int x, int y) {
    qDebug() << "[tool system] left button released";
    switch (selectedTool) {
        case Tool::BRUSH: // todo: strange case when the action isn't in a good state ?
            actionBuffer->front()->record(x, y);
            actionBuffer->front()->setState(ActionState::COMPLETE);
            emit actionsAvailable();
            break;
        default:
            break;
    }
}

void ToolSystem::mouseMoved(int x, int y) {
    // note: only triggered when left mouse button down
    switch (selectedTool) {
        case Tool::BRUSH:
            actionBuffer->front()->record(x, y);
            emit actionsAvailable();
            break;
        default:
            break;
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
