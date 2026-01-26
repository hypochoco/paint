//
//  events.h
//  main
//
//  Created by Daniel Cho on 1/22/26.
//

#pragma once

#include "paint/nodes.h"
#include "paint/actions.h"

struct Node; // forward declaration
struct BrushStrokeData; // forward declaration

struct Event {
    virtual ~Event() = default;
    virtual void build(Node* node) = 0;
};

struct CameraEvent : public Event {
    void build(Node* node) override;
};

struct BrushStrokeEvent : public Event {
    BrushStrokeData* brushStrokeData;
    BrushStrokeEvent(BrushStrokeData* brushStrokeData) : brushStrokeData(brushStrokeData) {}
    ~BrushStrokeEvent();
    void build(Node* node) override;
};
