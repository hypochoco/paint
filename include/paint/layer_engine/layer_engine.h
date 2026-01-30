//
//  layer_engine.h
//  main
//
//  Created by Daniel Cho on 1/30/26.
//

#include <engine/graphics/graphics.h>

class LayerEngine {
    
public:
    public void init();
    public void cleanup();
    
private:
    Graphics* graphics;
    
};
