//
//  events.h
//  main
//
//  Created by Daniel Cho on 1/22/26.
//

#pragma once

#include "paint/nodes.h"

struct Node; // forward declaration

struct Event {
    virtual ~Event() = default;
    virtual void build(Node* node) = 0;
};

struct CameraEvent : public Event {
    ~CameraEvent() override {
        qDebug() << "[camera event] destructor called";
    }
    void build(Node* node) override;
};

//struct BrushEvent : public Event {
//    BrushStroke* brushStroke;
//    BrushEvent(BrushStroke* brushStroke) : brushStroke(brushStroke) {}
//    void build(Node* node) override;
//};

//void BrushEvent::build(Node* node) {
//    node->children.push_back( new BrushNode { brushStroke });
//}
