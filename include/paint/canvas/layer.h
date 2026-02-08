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
    uint32_t id = nextId++;
    std::string name;
    
    VkImage image{};
    VkDeviceMemory imageMemory{};
    VkImageView imageView{};
        
private:
    inline static uint32_t nextId = 0;
};
