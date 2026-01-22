//
//  events.cpp
//  main
//
//  Created by Daniel Cho on 1/22/26.
//

#include "paint/events.h"

void CameraEvent::build(Node* node) {
    node->children.push_back(new CameraNode);
}
