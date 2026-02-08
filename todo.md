
# Current development notes

Painting application:

- [x] Get layer targets working

- [ ] Layer renaming saved to canvas data structure

- [ ] Handle multiple canvas windows
- [ ] Draw blank if there is no canvas   
- [ ] Background gray color
    
- [ ] Canvas 
    - [ ] Sync canvas and layer panel 

    - [ ] Arbitrary canvas size
    
    - [ ] Layers 
        - [x] layer engine source switching
        - [x] brush engine target switching 
        - [x] starting layer
        - [x] layers after starting layer are clear
        - [x] delete and reordering layers
        - [x] cleanup canvas creation and initialization
        
        - [ ] fix layers are clear after layer switching
    
    - [ ] Frontend interaction
        - [x] create frontend
        - [ ] connect frontend functionality 
        - [ ] bottom to top 
        - [ ] layer groups

    - [ ] Tiling
        - [ ] calculate brush and layer tiles to update
        
    - [ ] Undo
        - [ ] image backups 
            
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
