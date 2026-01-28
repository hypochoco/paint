//
//  action_data.h
//  main
//
//  Created by Daniel Cho on 1/21/26.
//

#pragma once

#include <cstdint>

struct ActionData {
    uint32_t id;
    virtual ActionData* clone() = 0;
    
protected:
    ActionData() : id(nextId++) {}
    ActionData(const ActionData& other) : id(other.id) {}
    
private:
    inline static uint32_t nextId = 0;
    
};
