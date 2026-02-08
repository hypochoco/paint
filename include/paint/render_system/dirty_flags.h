//
//  dirty_flags.h
//  main
//
//  Created by Daniel Cho on 1/8/26.
//

#pragma once

#include <vector>

#include <QDebug>

#include "paint/render_system/dirty_flag.h"

class DirtyFlags {
    
public:
    void init(uint32_t maxFramesInFlight) {
        camera.resize(maxFramesInFlight);
    }

    void set(DirtyFlag flag) {
        switch (flag) {
            case DirtyFlag::RESIZED:
                qDebug() << "[dirty flags] set resized flag";
                resized = true;
                camera.assign(camera.size(), true);
                break;
            case DirtyFlag::ACTIONS:
                // qDebug() << "[dirty flags] set actions flag";
                actions = true;
                break;
            case DirtyFlag::LAYER:
                layer = true;
                break;
            default:
                qDebug() << "[dirty flags] warning: flag not implemented";
                break;
        }
    }
    
    void clear(int frame) {
        resized = false;
        camera[frame] = false;
        actions = false;
        layer = false;
    }
    
    bool dirty() {
        return resized
        || std::any_of(camera.begin(), camera.end(), [](bool b){ return b; })
        || actions
        || layer;
    }
    
    bool dirty(DirtyFlag flag) {
        switch (flag) {
            case DirtyFlag::RESIZED:
                return resized;
            case DirtyFlag::ACTIONS:
                qDebug() << "[dirty flags] actions flag: " << actions;
                return actions;
            case DirtyFlag::LAYER:
                qDebug() << "[dirty flags] layer flag: " << layer;
                return layer;
            default:
                qDebug() << "[dirty flags] warning: flag not implemented";
                return false;
        }
    }
    
    bool dirty(DirtyFlag flag, int currentFrame) {
        switch (flag) {
            case DirtyFlag::CAMERA:
                qDebug() << "[dirty flags] camera flag: " << camera[currentFrame];
                return camera[currentFrame];
            default:
                qDebug() << "[dirty flags] warning: flag not implemented";
                return false;
        }
    }
    
private:
    bool resized;
    std::vector<bool> camera;
    bool actions;
    bool layer;

};
