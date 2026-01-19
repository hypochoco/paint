//
//  graph.h
//  paint
//
//  Created by Daniel Cho on 1/11/26.
//

#pragma once

#include <vector>

#include "paint/worker.h"
#include "paint/tool.h"

class RenderWorker; // forward declaration
struct BrushStroke; // forward delcaration

// nodes

struct Node {
    ~Node() = default;
    std::vector<Node*> children;
    virtual void execute(RenderWorker* worker) {};
};

struct CameraNode : public Node {
    void execute(RenderWorker* worker) override;
};

struct BrushNode : public Node {
    BrushStroke* brushStroke;
    BrushNode(BrushStroke* brushStroke) : brushStroke(brushStroke) {}
    void execute(RenderWorker* worker) override;
};

// events

struct Event {
    virtual ~Event() = default;
    virtual void build(Node* node) = 0;
};

struct CameraEvent : public Event {
    void build(Node* node) override;
};

struct BrushEvent : public Event {
    BrushStroke* brushStroke;
    BrushEvent(BrushStroke* brushStroke) : brushStroke(brushStroke) {}
    void build(Node* node) override;
};

// frame graph

struct FrameGraph {
    
    FrameGraph() {};
    
    uint32_t imageIndex;
    uint32_t currentFrame;
    
    float cx, cy, cz; // camera positions
    float windowAspect;
        
    Node* root;
    std::vector<Event*> events;
    
    void addCameraEvent() {
        events.emplace_back(new CameraEvent {});
    }
    
    void addBrushEvent(BrushStroke* brushStroke) {
        events.emplace_back(new BrushEvent { brushStroke });
    }
    
    void build() {
        root = new Node;
        for (Event* event : events) {
            event->build(root);
        }
    }

    size_t size() {
        return events.size();
    }
    
    bool empty() {
        return size() == 0;
    }
    
    void deleteNode(Node* node) { // note: recursive delete
        if (!node) return;
        for (Node* child : node->children) {
            deleteNode(child);
        }
        delete node;
    }
    
    void clear() {
        for (Event* event : events) {
            delete event;
        }
        events.clear();
        deleteNode(root);
        root = nullptr;
    }

};
