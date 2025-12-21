
# Painting Application

Graphics through Vulkan.

Potential names:
- Haze
- Arco
- Lumen
- Sable

## Build Instructions

**General build:**
- `./scripts/compile.sh` to compile (not meant for Xcode)
- `./scripts/compile_shaders.sh` to compile shaders to resources dir
- `./scripts/run.sh` to run program

Note: File paths have not been verified for non-Xcode builds--this will likely have to change.

Xcode specific instructions:
- Run `cmake -G Xcode ..` in the build directory and open Xcode project file,
- In `Build Phases`, add compiled files into `Compile Sources`, 
- Create a new `Copy Bundle Resource` and add shaders, textures, models, etc.,
- In `Build Settings`, add include/header dir to `Header Search Paths`.

For final compilation:
- Run your program now with optimization enabled (e.g. Release mode with the -O3 compiler flag for GCC`)
