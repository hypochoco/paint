//
//  application.cpp
//  engine
//
//  Created by Daniel Cho on 9/30/25.
//

#include "paint/application.h"

#include <stdexcept>
#include <iostream>
#include <chrono>

// paint application

Application::Application() { // constructor
    // currently empty
}

Application::~Application() { // destructor
    // cleanup here
}

void Application::init() {
    
    graphicsSystem.init();
    inputSystem.registerGLFW(graphicsSystem.graphics.window);
    
}

void Application::run() {
    
    // main loop
    
    graphicsSystem.draw();
    inputSystem.reset();
    
    auto last = std::chrono::steady_clock::now();
    
    // random paint stuff
    brushSize = 0.25f;
    
    // actual main loop
    while (!glfwWindowShouldClose(graphicsSystem.graphics.window)) {
        glfwPollEvents();
        
        // todo: move inputs into another thread
        
        // draw brush
        
        if (inputSystem.pressed) { // todo: better architecture
            graphicsSystem.bufferBrush(inputSystem.xpos,
                                       inputSystem.ypos,
                                       brushSize);
            inputSystem.reset();
        }
        
        // change brush size
        
        if (inputSystem.leftBracketPressed) {
            if (brushSize > 0.0f) {
                brushSize -= 0.01f;
            }
            inputSystem.reset();
        } else if (inputSystem.rightBracketPressed) {
            brushSize += 0.01f;
            inputSystem.reset();
        }
        
        // change zoom
        
        if (inputSystem.commaPressed) {
            graphicsSystem.depth += 0.5;
            graphicsSystem.initCamera();
            inputSystem.reset();
        } else if (inputSystem.periodPressed) {
            if (graphicsSystem.depth >= 1) {
                graphicsSystem.depth -= 0.5;
                graphicsSystem.initCamera();
            }
            inputSystem.reset();
        }
        
        // pan
    
        if (inputSystem.up) {
            graphicsSystem.cameraY += 0.5f;
            graphicsSystem.initCamera();
            inputSystem.reset();
        } else if (inputSystem.down) {
            graphicsSystem.cameraY -= 0.5f;
            graphicsSystem.initCamera();
            inputSystem.reset();
        } else if (inputSystem.left) {
            graphicsSystem.cameraX -= 0.5f;
            graphicsSystem.initCamera();
            inputSystem.reset();
        } else if (inputSystem.right) {
            graphicsSystem.cameraX += 0.5f;
            graphicsSystem.initCamera();
            inputSystem.reset();
        }
            
        // draw at 60 fps
        
        // todo: dirty flags
        // todo: swapchain currently draws everything ? draw just dirty tiles ?
        
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> delta = now - last;
        if (delta > std::chrono::milliseconds(16)){
            graphicsSystem.draw();
            last = now;
        }
        
    }
    vkDeviceWaitIdle(graphicsSystem.graphics.device);
    
    // todo: add logging for average draw call timing
        // start time
        // count
        // end time / count
    
}

void Application::cleanup() {
    
    graphicsSystem.cleanup();
    
}
