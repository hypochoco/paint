//
//  graph.h
//  paint
//
//  Created by Daniel Cho on 1/11/26.
//

#pragma once

#include <vector>

#include "paint/render.h"
#include "paint/worker.h"
#include "paint/tool.h"

class RenderWorker; // forward declarations
struct RenderJob; // forward declarations
struct BrushStroke; // forward declarations

// nodes

struct Node {
    std::vector<Node*> children;
    virtual void execute(RenderWorker* worker) {};
};

struct CameraNode : public Node {
    CameraNode(float x, float y, float z, float aspect)
        : x(x), y(y), z(z), aspect(aspect) {}
    float x, y, z, aspect;
    void execute(RenderWorker* worker) override;
};

struct BrushNode : public Node {
    float x, y, z;
    uint32_t width, height;
    BrushStroke brushStroke;
    BrushNode(float x, float y, float z,
              uint32_t width, uint32_t height,
              BrushStroke brushStroke)
        : x(x), y(y), z(z),
        width(width), height(height),
        brushStroke(brushStroke) {}
    void execute(RenderWorker* worker) override;
};

// events

struct Event {
    virtual void build(Node* node) {}
};

struct CameraEvent : public Event {
    CameraEvent(float x, float y, float z, float aspect)
        : x(x), y(y), z(z), aspect(aspect) {}
    float x, y, z, aspect;
    void build(Node* node) override;
};

struct BrushEvent : public Event {
    float x, y, z;
    uint32_t width, height;
    BrushStroke brushStroke;
    BrushEvent(float x, float y, float z,
               uint32_t width, uint32_t height,
               BrushStroke brushStroke)
        : x(x), y(y), z(z),
        width(width), height(height),
        brushStroke(brushStroke) {}
    void build(Node* node) override;
};

// frame graph

struct FrameGraph {
    
    FrameGraph() {};
        
    std::vector<Event*> events;
    std::vector<Node*> nodes;
    
    void add(Event* event) { // add events
        events.push_back(event);
    }
    
    void addCameraEvent(float x, float y, float z, float aspect) {
        add(new CameraEvent { x, y, z, aspect });
    }
    
    void addBrushEvent(float x, float y, float z,
                       uint32_t width, uint32_t height,
                       BrushStroke brushStroke) {
        add(new BrushEvent {
            x, y, z,
            width, height,
            brushStroke
        });
    }
    
    void build() {
        Node* root = new Node;
        for (Event* event : events) {
            event->build(root);
        }
        for (Node* node : root->children) { // todo: remove
            nodes.push_back(node);
        }
    }

    size_t size() {
        return events.size();
    }
    
    bool empty() {
        return size() == 0;
    }
    
    void clear() {
        for (Event* event : events) {
            delete event;
        }
        events.clear();
        
        for (Node* node : nodes) { // todo: tree deletion
            delete node;
        }
        nodes.clear();
    }

};
