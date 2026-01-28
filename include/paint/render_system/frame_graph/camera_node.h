//
//  camera_node.h
//  main
//
//  Created by Daniel Cho on 1/22/26.
//

#pragma once

#include "paint/render_system/frame_graph/node.h"

#include "paint/render_system/render_worker.h"

struct CameraNode : public Node {
    void process(FrameGraph& frameGraph, RenderWorker& renderWorker) override;
};
