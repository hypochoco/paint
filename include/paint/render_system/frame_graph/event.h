//
//  event.h
//  main
//
//  Created by Daniel Cho on 1/22/26.
//

#pragma once

struct FrameGraph; // forward declaration
class RenderWorker; // forward declaration

struct Event {
    virtual ~Event() = default;
    virtual void build(FrameGraph& frameGraph, RenderWorker& renderWorker) = 0;
};
