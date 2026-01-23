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

struct Action; // forward declaration
template<size_t Capacity>
class ActionBuffer; // forward declaration

enum Tool {
    BRUSH
};

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
    Tool selectedTool;
    ActionBuffer<75>* actionBuffer;
};
