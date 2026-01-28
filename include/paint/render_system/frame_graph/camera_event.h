//
//  camera_event.h
//  main
//
//  Created by Daniel Cho on 1/22/26.
//

#pragma once

#include "paint/render_system/frame_graph/event.h"

struct CameraEvent : public Event {
    void build(Node* node) override;
};
