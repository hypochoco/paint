//
//  zoom_tool.cpp
//  main
//
//  Created by Daniel Cho on 2/12/26.
//

#include "paint/tool_system/zoom_tool.h"

bool ZoomTool::leftButtonPressed(int x, int y) {
    return false;
}

bool ZoomTool::leftButtonReleased(int x, int y) {
    return false;
}

bool ZoomTool::mouseMoved(int x, int y) {
    return false;
}

bool ZoomTool::keyPressed(QKeyEvent* event) {
    switch(event->key()) {
        case Qt::Key_Minus:
            qDebug() << "[zoom tool] minus key pressed";
            emit updateCamera(glm::vec3(0.f, 0.f, -0.5f));
            break;
        case Qt::Key_Equal:
            qDebug() << "[zoom tool] equal key pressed";
            emit updateCamera(glm::vec3(0.f, 0.f, 0.5f));
            break;
        default:
            break;
    }
    return false;
}
