//
//  brush_tool.h
//  main
//
//  Created by Daniel Cho on 1/12/26.
//

#pragma once

#include "paint/tool_system/tool.h"
#include "paint/tool_system/tool_system_config.h"
#include "paint/tool_system/actions/action_buffer.h"

class BrushTool : public Tool {
    Q_OBJECT
    
public:
    BrushTool(ActionBuffer<ACTION_BUFFER_SIZE>* actionBuffer) : actionBuffer(actionBuffer) {}
    
    ActionBuffer<ACTION_BUFFER_SIZE>* actionBuffer;
    
    void onSelect() override {};
    void onDeselect() override {};

    bool leftButtonPressed(int x, int y) override;
    bool leftButtonReleased(int x, int y) override;
    bool mouseMoved(int x, int y) override;
    bool keyPressed(QKeyEvent* event) override {};
    
signals:
    void querySelectedLayer(std::function<void(int)> reply);
    void queryBrushSize(std::function<void(float)> reply);
    void queryBrushSpacing(std::function<void(float)> reply);
    void queryBrushOpacity(std::function<void(float)> reply);
    void queryBrushColor(std::function<void(glm::vec3)> reply);
    
};
