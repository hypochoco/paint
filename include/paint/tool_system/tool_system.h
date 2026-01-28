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

class ToolSystem : public QObject {
    Q_OBJECT
    
public:
    ToolSystem();
    ~ToolSystem();
    
public slots:
    void leftButtonPressed(int x, int y);
    void leftButtonReleased(int x, int y);
    void mouseMoved(int x, int y);
    
    void onQueryActions(std::function<void(Action*)> reply);
    
signals:
    void actionsAvailable();
    
private:
    ActionBuffer<ACTION_BUFFER_SIZE>* actionBuffer;
    Tool* tool; // todo: list / mapping of tools
    
    void selectTool() {}; // todo: deselect current, select new
};
