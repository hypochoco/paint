//
//  layer_event.cpp
//  paint
//
//  Created by Daniel Cho on 2/1/26.
//

#include "paint/render_system/frame_graph/layer_event.h"

#include "paint/render_system/frame_graph/layer_node.h"

void LayerEvent::build(Node* node) {
    node->children.push_back(new LayerNode);
}
