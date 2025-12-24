//
//  input_system.cpp
//  engine
//
//  Created by Daniel Cho on 12/5/25.
//

#include "paint/input_system.h"

#include <iostream>

void register_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    InputSystem* inputSystem = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
    if (!inputSystem) return;
    inputSystem->mouse_button_callback(button, action, mods);
}

void register_mouse_position_callback(GLFWwindow* window, double xpos, double ypos) {
    InputSystem* inputSystem = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
    if (!inputSystem) return;
    inputSystem->mouse_position_callback(xpos, ypos);
}

void register_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    InputSystem* inputSystem = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
    if (!inputSystem) return;
    inputSystem->key_callback(key, action);
}

void InputSystem::registerGLFW(GLFWwindow* window) {
    
    glfwSetWindowUserPointer(window, this);
    
    glfwSetMouseButtonCallback(window, register_mouse_button_callback);
    glfwSetCursorPosCallback(window, register_mouse_position_callback);
    glfwSetKeyCallback(window, register_key_callback);
    
}

void InputSystem::reset() {
    pressed = false;
    
    leftBracketPressed = false;
    rightBracketPressed = false;
    
    commaPressed = false;
    periodPressed = false;
    
    up = false;
    down = false;
    left = false;
    right = false;
}

void InputSystem::mouse_button_callback(int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        pressed = true;
    }
}

void InputSystem::mouse_position_callback(double xpos, double ypos) {
    this->xpos = xpos;
    this->ypos = ypos;
}

void InputSystem::key_callback(int key, int action) {
    
    // std::cout << "key: " << key << ", action: " << action << std::endl;
    
    if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS) {
        leftBracketPressed = true;
    } else if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS) {
        rightBracketPressed = true;
    } else if (key == GLFW_KEY_COMMA && action == GLFW_PRESS) {
        commaPressed = true;
    } else if (key == GLFW_KEY_PERIOD && action == GLFW_PRESS) {
        periodPressed = true;
    } else if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        up = true;
    } else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        down = true;
    } else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        left = true;
    } else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        right = true;
    }
 
}
