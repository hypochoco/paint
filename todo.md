
# Current development notes

Painting application:

- [ ] Canvas 

    - [ ] Layers 
        - [ ] requires brush engine to switch layer targets
        - [ ] requires layer drawing commands -> layer engine 
        - [x] Frontend interaction 


    - [ ] Tiling
        - [ ] in brush engine, calculate what tiles from what layers need updates
    - [ ] Undo
        - [ ] image backups 
        
- [ ] Fix the existing draw job system 
    
---

- [ ] Brush tool
    - [x] Get stamping back
    - [x] Interpolation
    - [ ] Undo action / image backups 
        - [ ] Switch between canvas images (per layer)
        - [ ] Copy image to another images (per layer)
        - [ ] Some kind of data structure for this ?
        - [ ] General canvas work, layers, tiling
    
    - [ ] More interpolation methods 
    - [ ] Round brush
    - [ ] Brush opacity
    - [ ] Brush settings UI

- [ ] Tiling
- [ ] Layer visibility 
- [ ] Canvas flipping
- [ ] Zoom tool
    
# Long term features

## Priority features

- PSD compatibility 
- Fast/responsive, customizable brush engine
    - Mixer brushes / blending
    - Stabilization
    - Textures
    - Brush node system ?
- Color Management
    - ICC profile support
    - CMYK preview (even if you don’t edit CMYK)
    - Soft proofing
    - Wide-gamut handling (Display P3, Adobe RGB)
    - 16-bit + 32-bit float modes
- Layer management system (200+ layers)
- Autosave and versioning system
    - Compare versions
    - Rollback, combine, cherry pick, etc.
- Customizable UI
- Common tools
    - Liquify, lasso, transform, etc.

## Secondary features

- Plugin support 
- Cross platform
- Collab / cloud
- AI features ?
