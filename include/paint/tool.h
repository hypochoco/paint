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

struct Tool {
    
    virtual ~Tool() = default;
            
    virtual void onSelect() = 0;
    virtual void onDeselect() = 0;

    virtual bool leftButtonPressed(int x, int y) = 0;
    virtual bool leftButtonReleased(int x, int y) = 0;
    virtual bool mouseMoved(int x, int y) = 0;
};

struct BrushTool : public Tool {
    
    BrushTool(ActionBuffer<75>* actionBuffer) : actionBuffer(actionBuffer) {}
    
    ActionBuffer<75>* actionBuffer;
    
    void onSelect() override {};
    void onDeselect() override {};

    bool leftButtonPressed(int x, int y) override;
    bool leftButtonReleased(int x, int y) override;
    bool mouseMoved(int x, int y) override;
    
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
    ActionBuffer<75>* actionBuffer;
    Tool* tool; // todo: list / mapping of tools
    
    void selectTool() {}; // todo: deselect current, select new
};
