//
//  tool.h
//  main
//
//  Created by Daniel Cho on 1/12/26.
//

#pragma once

#include <QObject>
#include <QDebug>

#include "paint/graph.h"

struct FrameGraph; // forward declaration

enum Tool {
    BRUSH
};

struct Action : QObject { // todo: make into a QObject
    Q_OBJECT
    
public:
    virtual ~Action() = default;
    
    bool complete = false;
    bool processed = false;
    
    virtual void start(int x, int y) = 0;
    virtual void record(int x, int y) = 0;
    virtual void end(int x, int y) = 0;
    
    virtual void addEvent(FrameGraph* frameGraph) = 0;
};

struct BrushPoint {
    int x, y; // todo: pressure
};

struct BrushStroke : public Action {
    std::vector<BrushPoint> rawBrushPoints;
    std::vector<BrushPoint> brushPoints;
    void addRawBrushPoint(int x, int y) {
        rawBrushPoints.emplace_back(BrushPoint { x, y });
    }
    void addBrushPoint(int x, int y) {
        brushPoints.emplace_back(BrushPoint { x, y });
    }
    void start(int x, int y) override {
        addRawBrushPoint(x, y);
    }
    void record(int x, int y) override {
        addRawBrushPoint(x, y);
    }
    void end(int x, int y) override {
        addRawBrushPoint(x, y);
        complete = true;
    }
    void addEvent(FrameGraph* frameGraph) override;
};

class ToolSystem : public QObject {
    Q_OBJECT
    
public:
    ToolSystem() {
        selectedTool = Tool::BRUSH; // temp: default tool
    }
    ~ToolSystem() {
        if (currentAction) {
            delete currentAction;
            currentAction = nullptr;
        }
        for (Action* action : actionHistory) {
            delete action;
        }
        actionHistory.clear();
    }
    
public slots:
    void leftButtonPressed(int x, int y) {
        qDebug() << "[tool system] left button pressed";
        switch (selectedTool) {
            case Tool::BRUSH:
                currentAction = new BrushStroke;
                break;
            default:
                break;
        }
        if (currentAction) {
            currentAction->start(x, y);
        }
    }
    void leftButtonReleased(int x, int y) {
        qDebug() << "[tool system] left button released";
        if (currentAction) {
            currentAction->end(x, y);
            actionHistory.push_back(currentAction);
            currentAction = nullptr;
        }
    }
    void mouseMoved(int x, int y) { // note: only triggered when left mouse button down
        if (currentAction) {
            currentAction->record(x, y);
        }
    }
    void onQuery() { // send over queued tools
        qDebug() << "[tool system] on query";
        std::vector<Action*> actions;
        if (currentAction && !currentAction->processed) {
            actions.push_back(currentAction);
        }
        for (Action* action : actionHistory) {
            if (!action->processed) {
                actions.push_back(action);
            }
        }
        if (!actions.empty()) {
            emit submitActions(actions);
        }
    }
    
signals:
    void submitActions(std::vector<Action*> actions);
    
private:
    Tool selectedTool;
    Action* currentAction;
    std::vector<Action*> actionHistory;
};
