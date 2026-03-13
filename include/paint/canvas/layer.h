//
//  layer.h
//  main
//
//  Created by Daniel Cho on 1/31/26.
//

#pragma once

#include <string>

#include <engine/graphics/graphics.h>

struct Layer {
    
    // don't need an id anymore ?? it's the quid ?? or should we have our own in here anyway ...
    // what's the point of this thing anyway ...
    
    
    
    
    
    uint32_t id = nextId++;
    std::string name;
    
    bool visible = true;
    // todo: opacity
    
    VkImage image{};
    VkDeviceMemory imageMemory{};
    VkImageView imageView{};
        
private:
    inline static uint32_t nextId = 0;
};
