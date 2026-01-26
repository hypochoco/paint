//
//  graph.h
//  paint
//
//  Created by Daniel Cho on 1/11/26.
//

#pragma once

#include <QDebug>

#include <cstdint>

#include "paint/events.h"
#include "paint/nodes.h"
#include "paint/camera.h"
#include "paint/actions.h"

struct Event; // forward declaration
struct Node; // forward declaration
struct BrushStrokeData; // forward declaration

struct FrameGraph {
    uint32_t currentFrame, imageIndex;
    int windowWidth, windowHeight;
    Camera camera;
    
    std::vector<Event*> events;
    Node* root;
    
    void build();
    void cleanup();
};

inline QDebug operator<<(QDebug dbg, const FrameGraph &fg) {
    QDebugStateSaver saver(dbg);
    dbg.nospace()
        << "FrameGraph{"
        << "\n\tcurrentFrame=" << fg.currentFrame
        << ", imageIndex=" << fg.imageIndex
        << ", \n\twindowWidth=" << fg.windowWidth
        << ", windowHeight=" << fg.windowHeight
        << ", \n\tcamera=" << fg.camera
        << ", \n\tevents size=" << fg.events.size()
        << "}";
    return dbg;
}

struct FrameGraphBuilder {
    
    uint32_t imageIndex, currentFrame;
    int windowWidth, windowHeight;
    Camera camera;
    std::vector<Event*> events;
    
    FrameGraphBuilder& withImageIndex(uint32_t imageIndex) {
        this->imageIndex = imageIndex;
        return *this;
    };
    
    FrameGraphBuilder& withCurrentFrame(uint32_t currentFrame) {
        this->currentFrame = currentFrame;
        return *this;
    };
    
    FrameGraphBuilder& withWindowWidth(int width) {
        this->windowWidth = width;
        return *this;
    };

    FrameGraphBuilder& withWindowHeight(int height) {
        this->windowHeight = height;
        return *this;
    };
    
    FrameGraphBuilder& withCamera(Camera camera) {
        this->camera = camera;
        return *this;
    };
    
    FrameGraphBuilder& addCameraEvent();
    FrameGraphBuilder& addBrushStrokeEvent(BrushStrokeData* brushStrokeData);

    FrameGraph build() {
        return FrameGraph {
            currentFrame,
            imageIndex,
            windowWidth,
            windowHeight,
            camera,
            events
        };
    }
};
