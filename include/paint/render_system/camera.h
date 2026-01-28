//
//  camera.h
//  main
//
//  Created by Daniel Cho on 1/22/26.
//

#pragma once

#include <QDebug>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// todo: allow other methods like ray tracing

struct Camera {
    glm::vec3 position;
    
    void calculateMatrices(glm::mat4& view, glm::mat4& proj, const glm::vec2& windowSize) {
        view = glm::lookAt(position, // camera pos
                           glm::vec3(position.x,
                                     position.y,
                                     0.0f), // look at
                           glm::vec3(0.0f, 1.0f, 0.0f)); // up
        proj = glm::perspective(glm::radians(45.0f), // fovy
                                windowSize.x / windowSize.y,
                                0.1f, // near
                                10.0f); // far
        proj[1][1] *= -1; // strange projection fix
    }
    
    glm::vec2 screenToWorldSpace(glm::vec2 windowSize,
                                 glm::vec2 point) {
        float aspect = windowSize.x / windowSize.y; // window aspect
        static float tanHalfFovy = 0.4142f; // hard coded for 45 deg
        
        float ndcX = (point.x / windowSize.x) * 2.0f - 1.0f;
        float ndcY = 1.0f - (point.y / windowSize.y) * 2.0f;

        return {
            ndcX * position.z * tanHalfFovy * aspect + position.x,
            ndcY * position.z * tanHalfFovy + position.y
        };
    }
};

inline QDebug operator<<(QDebug dbg, const Camera &c) {
    QDebugStateSaver saver(dbg);
    dbg.nospace()
        << "Camera{"
        << "position=(" << c.position.x << ", " << c.position.y << ", " << c.position.z << ")"
        << "}";
    return dbg;
}
