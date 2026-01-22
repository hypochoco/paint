//
//  camera.h
//  main
//
//  Created by Daniel Cho on 1/22/26.
//

#pragma once

#include <glm/glm.hpp>

struct Camera {
    glm::vec3 position;
};

inline QDebug operator<<(QDebug dbg, const Camera &c) {
    QDebugStateSaver saver(dbg);
    dbg.nospace()
        << "Camera{"
        << "position=(" << c.position.x << ", " << c.position.y << ", " << c.position.z << ")"
        << "}";
    return dbg;
}
