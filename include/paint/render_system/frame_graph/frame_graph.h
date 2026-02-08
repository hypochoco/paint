//
//  frame_graph.h
//  paint
//
//  Created by Daniel Cho on 1/11/26.
//

#pragma once

#include <QDebug>

#include <cstdint>
#include <vector>

#include "paint/render_system/camera.h"
#include "paint/canvas/canvas_data.h"
#include "paint/render_system/frame_graph/event.h"

struct Event; // forward declaration
struct Node; // forward declaration

struct FrameGraph {
    
    uint32_t currentFrame, imageIndex;
    glm::vec2 windowSize;
    
    Camera camera;
    CanvasData canvasData;
    int selectedLayer = -1;
    
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
        << ", \n\twindowWidth=" << fg.windowSize.x
        << ", windowHeight=" << fg.windowSize.y
        << ", \n\tcamera=" << fg.camera
        << ", \n\tselectedLayer=" << fg.selectedLayer
        << ", \n\tevents size=" << fg.events.size()
        << "}";
    return dbg;
}
