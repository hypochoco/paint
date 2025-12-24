//
//  application.h
//  engine
//
//  Created by Daniel Cho on 9/30/25.
//

#pragma once

#include "input_system.h"
#include "graphics_system.h"

// paint application

class Application {
    
private:
    
    float brushSize;
        
    InputSystem inputSystem;
    GraphicsSystem graphicsSystem;
    
public:
    
    Application();
    ~Application();

    void init();
    void run();
    void cleanup();

};
