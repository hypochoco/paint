//
//  event.h
//  main
//
//  Created by Daniel Cho on 1/22/26.
//

#pragma once

#include "paint/render_system/frame_graph/node.h"

struct Node; // forward declaration

struct Event {
    virtual ~Event() = default;
    virtual void build(Node* node) = 0;
};
