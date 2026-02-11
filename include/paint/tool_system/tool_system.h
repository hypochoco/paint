//
//  tool_system.h
//  main
//
//  Created by Daniel Cho on 1/12/26.
//

#pragma once

#include <QObject>
#include <QDebug>

#include "paint/tool_system/tool.h"
#include "paint/tool_system/tool_system_config.h"
#include "paint/tool_system/actions/action.h"
#include "paint/tool_system/actions/action_state.h"
#include "paint/tool_system/actions/action_buffer.h"

#include "paint/tool_system/brush_tool.h"

class ToolSystem : public QObject {
    Q_OBJECT
    
public:
    ToolSystem();
    ~ToolSystem();
    
    BrushTool* brushTool;
    
public slots:
    void leftButtonPressed(int x, int y);
    void leftButtonReleased(int x, int y);
    void mouseMoved(int x, int y);
    
    void onQueryActions(std::function<void(Action*)> reply);
    
signals:
    void actionsAvailable();
    
private:
    Tool* selectedTool;
    ActionBuffer<ACTION_BUFFER_SIZE>* actionBuffer;
    
    void selectTool(Tool* tool);
};
