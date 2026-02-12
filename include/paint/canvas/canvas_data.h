//
//  canvas_data.h
//  main
//
//  Created by Daniel Cho on 2/3/26.
//

#pragma once

#include <QDebug>

#include "paint/canvas/layer.h"

struct CanvasData {
    static constexpr int TILE_WIDTH = 512;
    static constexpr int TILE_HEIGHT = 512;
    
    int width, height;
    float aspect;
    
    
    
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
