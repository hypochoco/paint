//
//  actions.h
//  main
//
//  Created by Daniel Cho on 1/21/26.
//

#pragma once

#include <array>
#include <glm/glm.hpp>

enum ActionState {
    RECORDING,
    COMPLETE,
    PROCESSED
};

struct Action {
    Action() : actionState(ActionState::RECORDING) {}
    virtual ~Action() = default;

    ActionState actionState;
    
    virtual Action* clone() = 0;
    
};

template<size_t Capacity>
class ActionBuffer {
    // todo: once this hits capacity clear ? or remove from back some number ...

public:
    ~ActionBuffer() {
        // clear(Capacity); // todo
    }
    
    // todo: functions
    
private:
    size_t head, tail;
    std::array<Action*, Capacity> buffer{};
    
};

struct BrushPoint {
    glm::vec2 position;
};

struct BrushStroke : public Action {
        
    BrushStroke* clone() override {
        return new BrushStroke(*this);
    }
    
};
