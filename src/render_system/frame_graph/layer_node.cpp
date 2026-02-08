//
//  layer_node.cpp
//  paint
//
//  Created by Daniel Cho on 2/1/26.
//

#include "paint/render_system/frame_graph/layer_node.h"

void LayerNode::process(FrameGraph& frameGraph, RenderWorker& renderWorker) {
    renderWorker.processLayerNode(frameGraph);
}
