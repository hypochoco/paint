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

struct Action {
    
    virtual ~Action() = default;

    bool processed = false;
    
    virtual Action* clone() = 0;

    virtual void start(int x, int y) = 0;
    virtual void record(int x, int y) = 0;
    virtual void end(int x, int y) = 0;
    
    virtual void update() = 0;
    virtual void addEvent(FrameGraph* frameGraph) = 0;
    
};

struct BrushPoint {
    BrushPoint(int x, int y) : position(x,y) {}
    BrushPoint(glm::vec2 position) : position(position) {}
    glm::vec2 position;
};

struct BrushStroke : public Action {
    
    size_t submitedIndex;
    std::vector<BrushPoint> rawBrushPoints;
    
    BrushStroke* clone() override {
        return new BrushStroke(*this);
    }
    
    void addRawBrushPoint(int x, int y) {
        rawBrushPoints.emplace_back(BrushPoint(x,y));
    }
    void start(int x, int y) override {
        addRawBrushPoint(x, y);
    }
    void record(int x, int y) override {
        addRawBrushPoint(x, y);
    }
    void end(int x, int y) override {
        addRawBrushPoint(x, y);
    }
    void update() override {
        submitedIndex = rawBrushPoints.size()-1;
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
        std::vector<Action*> actions; // copy
        if (currentAction && !currentAction->processed) {
            actions.push_back(currentAction->clone());
            currentAction->update();
        }
        for (Action* action : actionHistory) {
            if (!action->processed) {
                action->processed = true; // note: assuming completion
                actions.push_back(action->clone());
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
