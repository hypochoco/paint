//
//  tool_system.cpp
//  main
//
//  Created by Daniel Cho on 1/12/26.
//

#include "paint/tool_system/tool_system.h"

ToolSystem::ToolSystem() {
    actionBuffer = new ActionBuffer<ACTION_BUFFER_SIZE>;
    brushTool = new BrushTool { actionBuffer };
    zoomTool = new ZoomTool { actionBuffer };
    
    selectTool(brushTool); // note: default tool
}

ToolSystem::~ToolSystem() {
    selectedTool = nullptr;
    delete brushTool;
    delete zoomTool;
    delete actionBuffer;
}

void ToolSystem::selectTool(Tool* tool) {
    if (selectedTool) selectedTool->onDeselect();
    selectedTool = tool;
    if (selectedTool) selectedTool->onSelect();
}

void ToolSystem::keyPressed(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_B:
            qDebug() << "[tool system] selected brush tool";
            selectTool(brushTool);
            break;
        case Qt::Key_Z:
            qDebug() << "[tool system] selected zoom tool";
            selectTool(zoomTool);
            break;
        default:
            if (!selectedTool) break;
            if (selectedTool->keyPressed(event)) {
                emit actionsAvailable();
            }
            break;
    }
}

void ToolSystem::leftButtonPressed(int x, int y) {
    qDebug() << "[tool system] left button pressed";
    if (!selectedTool) return;
    if (selectedTool->leftButtonPressed(x, y)) {
        emit actionsAvailable();
    }
}

void ToolSystem::leftButtonReleased(int x, int y) {
    qDebug() << "[tool system] left button released";
    if (!selectedTool) return;
    if (selectedTool->leftButtonReleased(x, y)) {
        emit actionsAvailable();
    }
}

void ToolSystem::mouseMoved(int x, int y) {
    // note: only triggered when left mouse button down
    if (!selectedTool) return;
    if (selectedTool->mouseMoved(x, y)) {
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
