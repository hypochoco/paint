//
//  tool.h
//  main
//
//  Created by Daniel Cho on 1/12/26.
//

#pragma once

#include <QObject>
#include <QDebug>

#include "paint/actions.h"

enum Tool {
    BRUSH
};

class ToolSystem : public QObject {
    Q_OBJECT
    
public:
    ToolSystem() {
        selectedTool = Tool::BRUSH; // temp: default tool
    }
    
public slots:
    void leftButtonPressed(int x, int y);
    void leftButtonReleased(int x, int y);
    void mouseMoved(int x, int y);
    
    void onQueryActions();
    
private:
    Tool selectedTool;
    ActionBuffer<150> actionBuffer;
};
