//
//  tool.cpp
//  main
//
//  Created by Daniel Cho on 1/12/26.
//

#include "paint/tool.h"

void ToolSystem::leftButtonPressed(int x, int y) {
    qDebug() << "[tool system] left button pressed";
    switch (selectedTool) {
        case Tool::BRUSH:
            break;
        default:
            break;
    }
}

void ToolSystem::leftButtonReleased(int x, int y) {
    qDebug() << "[tool system] left button released";
}

void ToolSystem::mouseMoved(int x, int y) {
    // note: only triggered when left mouse button down
    
}

void ToolSystem::onQueryActions() {
    // todo
}
