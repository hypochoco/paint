//
//  canvas.h
//  main
//
//  Created by Daniel Cho on 12/24/25.
//

#pragma once

#include <QWindow>
#include <QWidget>
#include <QTimer>
#include <QVulkanInstance>

#include <QExposeEvent>
#include <QEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>

class Canvas {
    // todo: struct that holds some data on size and other stuff
};

class CanvasWindow : public QWindow {
    Q_OBJECT
    
public:
    CanvasWindow(QVulkanInstance* inst) {
        setVulkanInstance(inst);
        setSurfaceType(QSurface::VulkanSurface);
    }
    
    void exposeEvent(QExposeEvent *event) override;
    bool event(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent *event) override;

public slots:
    void onQueryWindowSize(std::function<void(int, int)> reply) {
        reply(width(), height());
    }
    void onRequestUpdate() {
        qDebug() << "[canvas] update requested";
        requestUpdate();
    }
    
signals:
    void surfaceCreated(QWindow* window);
    void surfaceAboutToBeDestroyed();
    void resized(int width, int height);
    void exposed(bool isExposed);
    
    void leftButtonPressed(int x, int y);
    void leftButtonReleased(int x, int y);
    void mouseMoved(int x, int y);
        
private:
    bool leftMouseDown = false;
    
};
