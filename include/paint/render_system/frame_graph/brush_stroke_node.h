//
//  brush_stroke_node.h
//  main
//
//  Created by Daniel Cho on 1/22/26.
//

#pragma once

#include "paint/render_system/frame_graph/node.h"
#include "paint/tool_system/actions/brush_stroke_data.h"

#include "paint/render_system/render_worker.h"

struct BrushStrokeNode : public Node {
    std::unordered_map<glm::ivec2, std::vector<BrushPoint>> canvasMap;
    BrushStrokeNode(std::unordered_map<glm::ivec2, std::vector<BrushPoint>> canvasMap)
    : canvasMap(canvasMap) {}
    void process(FrameGraph& frameGraph, RenderWorker& renderWorker) override;
};
