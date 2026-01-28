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

struct BrushTool : public Tool {
    
    BrushTool(ActionBuffer<ACTION_BUFFER_SIZE>* actionBuffer) : actionBuffer(actionBuffer) {}
    
    ActionBuffer<ACTION_BUFFER_SIZE>* actionBuffer;
    
    void onSelect() override {};
    void onDeselect() override {};

    bool leftButtonPressed(int x, int y) override;
    bool leftButtonReleased(int x, int y) override;
    bool mouseMoved(int x, int y) override;
    
};
