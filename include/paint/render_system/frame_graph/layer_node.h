//
//  layer_node.h
//  paint
//
//  Created by Daniel Cho on 2/1/26.
//

#pragma once

#include "paint/render_system/frame_graph/node.h"

#include "paint/render_system/render_worker.h"
#include "paint/render_system/tile.h"

struct LayerNode : public Node {
    std::vector<Tile> tiles;
    LayerNode() {}
    LayerNode(std::vector<Tile> tiles) : tiles(tiles) {}
    void process(FrameGraph& frameGraph, RenderWorker& renderWorker) override;
};
