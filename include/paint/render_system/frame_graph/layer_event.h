//
//  layer_event.h
//  paint
//
//  Created by Daniel Cho on 2/1/26.
//

#pragma once

#include "paint/render_system/frame_graph/event.h"

struct LayerEvent : public Event {
    void build(Node* node) override;
};
