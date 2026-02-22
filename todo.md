
# Current development notes

Painting application:

- [ ] Bug: Brushes on different layers are not merging correctly
- [ ] Bug: Purple flicker
- [ ] Bug: Brush / Layer engine, not drawing at edges
- [ ] Bug: Sometimes, drawing between tiles gets clipped
    - [ ] Seen once on 2050x2050 canvas size
    - [ ] Seen pretty often now
    - [ ] Fix canvas sizing, make multiple quads for this 
- [ ] Bug: Culling distance on zoom

---

- [ ] figure out some basic styling
- [ ] figure out render pipeline and everything
    - [ ] for opacity and colors

--- 

- [ ] More brush features
    - [ ] Colors
    - [ ] Opacity
- [ ] Color picker optimization 
- [ ] Clean up the brush engine
    - [ ] Better way of handling canvas size / space, world space
- [ ] Handle multiple canvas windows
    - [ ] Dockable canvas widget 
- [ ] Background gray color
- [ ] Zoom tool
    - [x] keyboard zoom in and out
    - [ ] mouse zoom in and out
- [ ] Tool selection indicator

- [ ] Colors panel 
    - [ ] Color selection
    - [ ] Pass into brush stroke data 

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
        
    - [ ] Layer visibility 

- [ ] Layer panel
    - [x] create frontend
    - [x] connect frontend functionality 
    - [ ] bottom to top 
    - [ ] layer groups -> use a QTreeWidget instead of a list
    
- [x] Brush panel
    - [x] Panel creation
    - [x] Panel functionality 
        - [x] brush size and spacing
    
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
            
- [ ] Brush tool
    - [x] Get stamping back
    - [x] Interpolation
    - [ ] Undo action / image backups 
        - [x] Switch between canvas images (per layer)
        - [ ] Copy image to another images (per layer)
        - [x] Some kind of data structure for this ?
        - [x] General canvas work, layers, tiling
    
    - [ ] More interpolation methods 
    - [ ] Round brush
    - [ ] Brush opacity
    - [x] Brush settings UI

- [ ] Canvas flipping
    
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
