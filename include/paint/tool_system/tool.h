//
//  tool.h
//  main
//
//  Created by Daniel Cho on 1/12/26.
//

#pragma once

#include <QObject>
#include <QKeyEvent>

class Tool : public QObject {
    Q_OBJECT
    
public:
    virtual ~Tool() = default;
            
    virtual void onSelect() = 0;
    virtual void onDeselect() = 0;

    virtual bool leftButtonPressed(int x, int y) = 0;
    virtual bool leftButtonReleased(int x, int y) = 0;
    virtual bool mouseMoved(int x, int y) = 0;
    virtual bool keyPressed(QKeyEvent* event) = 0;
    
};
