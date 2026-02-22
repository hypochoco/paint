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
    
    int selectedLayer = -1;
    float brushSize = 0.05f;
    float brushSpacing = 0.01f;
    float brushOpacity = 0.8f;
    glm::vec3 brushColor = glm::vec3(0, 0, 0);
    
    emit querySelectedLayer([&selectedLayer](int sl) {
        selectedLayer = sl;
    });
    emit queryBrushSize([&brushSize](float bs) {
        brushSize = bs;
    });
    emit queryBrushSpacing([&brushSpacing](float bs) {
        brushSpacing = bs;
    });
    emit queryBrushOpacity([&brushOpacity](float bo) {
        brushOpacity = bo;
    });
    emit queryBrushColor([&brushColor](glm::vec3 bc) {
        brushColor = bc;
    });
        
    BrushStroke* action = new BrushStroke;
    action->data.selectedLayer = selectedLayer;
    action->data.brushSize = brushSize;
    action->data.brushSpacing = brushSpacing;
    action->data.color = glm::vec4(brushColor, brushOpacity);
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
