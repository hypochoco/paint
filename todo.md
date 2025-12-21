## Todo

- [ ] Address these vulkan issues:
    - [ ] Run your program now with optimization enabled (e.g. Release mode with the -O3 compiler flag for GCC`)
    
- [ ] Generalize vulkan features
    - [ ] Onboard onto QT
    
    - [ ] Move camera information into a class
    - [ ] More API-like, external applications that use this should not be exposed to any vulkan?
        - [ ] Potentially wrappers around everything 

- [ ] Tiling for digital painting with dirty flags 
    
- [ ] (Bug) Window resizing requires two draw calls to fix  
 
- [ ] Application level
    - [ ] Layer visibility 
    - [ ] Configure scissors
    - [ ] Brush functions / mouse down and released
    - [ ] Cut tool + transforms
    
- [ ] Make another application for the spinning model, keep engine general enough to support both functions 
    
- [ ] Get rid of a lot of the configs for inputs instead 





**General build:**
- `./scripts/compile.sh` to compile (not meant for Xcode)
- `./scripts/compile_shaders.sh` to compile shaders to resources dir
- `./scripts/run.sh` to run program

For Xcode, run `cmake -G Xcode ..` in the build directory and open Xcode project file.
- For macOS, set macOS bool in `GraphicsConfig` in `config.h`

Xcode specific instructions:
- In `Build Phases`, add compiled files into `Compile Sources`, and then shaders, textures, models, etc. into a `Copy Bundle Resource`
- In `Build Settings`, add dir paths to `Header Search Paths`
