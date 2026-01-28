//
//  action_buffer.h
//  main
//
//  Created by Daniel Cho on 1/21/26.
//

#pragma once

#include <array>
#include <cstddef>

#include "paint/tool_system/actions/action.h"

template<size_t Capacity>
class ActionBuffer {
    // todo: once this hits capacity clear, notify and clear?
public:
    ActionBuffer() : head(0), tail(0) {}
    ~ActionBuffer() {
        for (Action* action : buffer) {
            if (action) delete action;
        }
    }
    
    size_t head, tail;
    static constexpr size_t capacity = Capacity;
    
    void push(Action* action) noexcept {
        if (buffer[head]) {
            delete buffer[head];
        }
        buffer[head] = (action);
        head = (head + 1) % Capacity;
        if (head == tail) {
            tail = (tail + 1) % Capacity;
        }
    }
    
    Action* front() const {
        return buffer[(head + Capacity - 1) % Capacity];
    }
    
    size_t size() const {
        return (head + Capacity - tail) % Capacity;
    }
    
    Action* operator[](size_t i) {
        return buffer[(tail + i) % Capacity];
    }
        
private:
    std::array<Action*, Capacity> buffer{};
};
