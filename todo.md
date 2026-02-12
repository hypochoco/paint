
# Current development notes

Painting application:

- [ ] Bug: Brushes on different layers are not merging correctly
- [ ] Bug: Purple flicker
- [ ] Bug: Brush / Layer engine, not drawing at edges

- [ ] Clean up the brush engine

- [ ] Handle multiple canvas windows
- [ ] Background gray color

- [ ] Better way of handling canvas size / space, world space

- [ ] Brush panel

- [ ] Layers 
    - [x] Sync canvas and layer panel 
    - [x] layer engine source switching
    - [x] brush engine target switching 
    - [x] starting layer
    - [x] layers after starting layer are clear
    - [x] delete and reordering layers
    - [x] cleanup canvas creation and initialization
    - [x] fix layers are clear after layer switching
    - [x] White around brush edges 
    
    - [ ] Layer renaming saved to canvas data structure
    - [ ] Crash on deleting the background layer 
        - [ ] Draw blank if there is no canvas   

- [ ] Layer panel
    - [x] create frontend
    - [x] connect frontend functionality 
    - [ ] bottom to top 
    - [ ] layer groups
    
- [ ] Undo
    - [ ] image backups 
    
- [x] Rendering from top down on affected layers
- [x] Layer deletion
- [x] Get layer targets working

- [x] Tiling
    - [x] arbitrary sized canvas
    - [x] fix mouse + brush size positions
    - [x] general tiling system
        - [x] tile location calculation
    - [x] Move selected layer into the brush stroke action
            
---

- [ ] Brush tool
    - [x] Get stamping back
    - [x] Interpolation
    - [ ] Undo action / image backups 
        - [ ] Switch between canvas images (per layer)
        - [ ] Copy image to another images (per layer)
        - [x] Some kind of data structure for this ?
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
