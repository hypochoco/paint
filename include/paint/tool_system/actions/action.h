//
//  actions.h
//  main
//
//  Created by Daniel Cho on 1/21/26.
//

#pragma once

#include "paint/tool_system/actions/action_state.h"
#include "paint/render_system/frame_graph/frame_graph_builder.h"

struct Action {
    Action() : state(ActionState::RECORDING) {}
    virtual ~Action() = default;

    ActionState state;
    
    void setState(ActionState state) {
        this->state = state;
    }
    
    virtual void addEvent(FrameGraphBuilder& builder) = 0;
    virtual void record(int x, int y) = 0;
    virtual void update() = 0;
    
};
