//
//  layer_node.h
//  paint
//
//  Created by Daniel Cho on 2/1/26.
//

#pragma once

#include "paint/render_system/frame_graph/node.h"

#include "paint/render_system/render_worker.h"

struct LayerNode : public Node {
    void process(FrameGraph& frameGraph, RenderWorker& renderWorker) override;
};
