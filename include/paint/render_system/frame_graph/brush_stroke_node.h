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
#include "paint/render_system/tile.h"

struct BrushStrokeNode : public Node {
    std::vector<Tile> tiles;
    BrushStrokeNode(std::vector<Tile> tiles) : tiles(tiles) {}
    void process(FrameGraph& frameGraph, RenderWorker& renderWorker) override;
};
