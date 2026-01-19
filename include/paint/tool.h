//
//  tool.h
//  main
//
//  Created by Daniel Cho on 1/12/26.
//

#pragma once

#include <QObject>
#include <QDebug>

// todo: clean up the history here somehow

enum Tool {
    BRUSH
};

struct Action {
    
};

struct RawBrushPoint {
    int x, y;
    // todo: pressure
};

struct BrushStroke : public Action {
    uint32_t lastUpdated = 0;
    std::vector<RawBrushPoint> rawBrushPoints;
    
    bool ready() {
        if (rawBrushPoints.empty()) return false;
        else return lastUpdated != rawBrushPoints.size();
    }
};

class ToolSystem : public QObject {
    Q_OBJECT
    
public:
    ToolSystem() {
        selectedTool = Tool::BRUSH;
    }
    
public slots:
    void leftButtonPressed(int x, int y) {
        qDebug() << "[tool system] left button pressed";
        if (selectedTool == Tool::BRUSH) {
            brushStrokes.emplace_back(BrushStroke {});
            brushStrokes.back().rawBrushPoints.emplace_back(RawBrushPoint { x, y });
        }
    }
    void leftButtonReleased(int x, int y) {
        qDebug() << "[tool system] left button released";
        if (selectedTool == Tool::BRUSH) {
            brushStrokes.back().rawBrushPoints.emplace_back(RawBrushPoint { x, y });
        }
    }
    void mouseMoved(int x, int y) { // note: only triggered when left mouse button down
        if (selectedTool == Tool::BRUSH) {
            brushStrokes.back().rawBrushPoints.emplace_back(RawBrushPoint { x, y });
        }
    }
    void onQuery() { // send over queued tools
        if (!brushStrokes.empty() && brushStrokes.back().ready()) {
            
            qDebug() << "[tool] on query";
            
            emit submitBrushStroke(brushStrokes.back());
            brushStrokes.back().lastUpdated = (uint32_t) brushStrokes.back().rawBrushPoints.size();
        }
    }
    
signals:
    void submitBrushStroke(BrushStroke brushStroke);
    
private:
    Tool selectedTool;
    std::vector<BrushStroke> brushStrokes; // todo: ring buffer
};
