//
//  canvas.h
//  main
//
//  Created by Daniel Cho on 12/24/25.
//

#pragma once

#include <QVulkanInstance>
#include <QWindow>

#include <QEvent>
#include <QResizeEvent>
#include <QExposeEvent>
#include <QMouseEvent>

#include <engine/graphics/graphics.h>

#include "paint/utils.h"

class VulkanWindow : public QWindow {
    Q_OBJECT
    
public:
    
    bool dirty = true;
    
    Graphics* graphics;
    
    VulkanWindow(QVulkanInstance* inst) { // todo: consider creating instance here
        setVulkanInstance(inst);
        setSurfaceType(QSurface::VulkanSurface);
        
        graphics = new Graphics();
    }
    
    ~VulkanWindow() {
        delete graphics;
    }
            
    void init();
    void releaseSwapChain();
    void recreateSwapChain();
    void recordCommandBuffer();
    void render();
    void cleanup();
    
    void exposeEvent(QExposeEvent* event) override;
    bool event(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    
    void mousePressEvent(QMouseEvent* event) override;
    
    // todo: render functions
    // todo: swapchain resizing callbacks
    
private:
    // todo
    
};
