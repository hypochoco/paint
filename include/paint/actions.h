//
//  actions.h
//  main
//
//  Created by Daniel Cho on 1/21/26.
//

#pragma once

#include <array>
#include <vector>
#include <glm/glm.hpp>

#include "paint/graph.h"

struct FrameGraphBuilder; // forward declaration

enum ActionState {
    RECORDING,
    COMPLETE,
    PROCESSED
};

struct ActionData {
    // todo
    // todo: clone function ... 
};

struct Action {
    Action() : state(ActionState::RECORDING) {}
    virtual ~Action() = default;

    ActionState state;
        
    virtual void record(int x, int y) = 0;
    virtual void update() = 0;
    virtual void setState(ActionState state) {
        this->state = state;
    }
    
    virtual void addEvent(FrameGraphBuilder& builder) = 0;
};

struct BrushPoint {
    glm::vec2 position;
};

struct BrushStrokeData : public ActionData {
    std::vector<BrushPoint> brushPoints;
};

struct BrushStroke : public Action {
    
    BrushStrokeData data;
            
    void record(int x, int y) override {
        data.brushPoints.emplace_back(BrushPoint { glm::vec2(x,y) });
    }
    
    void update() override {} // todo: some kind of indication of processed
    
    void addEvent(FrameGraphBuilder& builder) override {}
    
};

template<size_t Capacity>
class ActionBuffer {
    // todo: once this hits capacity clear, notify and clear?
public:
    ActionBuffer() : head(0), tail(0) {}
    ~ActionBuffer() {}
    
    size_t head, tail;
    static constexpr size_t capacity = Capacity;
    
    void push(Action* action) noexcept {
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
