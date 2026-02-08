//
//  canvas_data.h
//  main
//
//  Created by Daniel Cho on 2/3/26.
//

#pragma once

#include "paint/canvas/layer.h"

struct CanvasData {
    int width, height;
    std::vector<Layer> layers;
};

inline QDebug operator<<(QDebug dbg, const CanvasData &cd) {
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "CanvasData:";
    for (const Layer& layer : cd.layers) {
        dbg.nospace() << "\n\t" << layer.name;
    }
    return dbg;
}
