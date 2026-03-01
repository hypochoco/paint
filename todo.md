
# Current development notes

Painting application:

- [ ] Bug: Culling distance on zoom

--- 

- [ ] Layer tree structure
- [ ] Layer visibility 

- [ ] Layer panel
    - [x] create frontend
    - [x] connect frontend functionality 
    - [ ] bottom to top 
    - [ ] layer groups -> use a QTreeWidget instead of a list

---

- [ ] Option to create a new canvas
- [ ] Fix initialization ordering, make more stable 

- [ ] Color picker optimization 
- [ ] Handle multiple canvas windows
    - [ ] Dockable canvas widget 
    - [ ] Overhaul of widget structure and general styling
- [ ] Background gray color - clear color in the swapchain pipeline
- [ ] Zoom tool
    - [x] keyboard zoom in and out
    - [ ] mouse zoom in and out
- [ ] Tool selection indicator

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
        - [ ] Draw blank if there is no none   
    
- [ ] Undo action
    - [ ] image backups 

- [x] figure out some basic styling
- [ ] complete styling
            
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
    - [x] Brush opacity
    - [x] Brush settings UI

- [ ] Canvas flipping

---

- [x] Clean up brush and layer drawing
    - [x] Flag or a function to enable / disable debug

- [x] Bug: Brushes on different layers are not merging correctly

- [x] More brush features
    - [x] Colors
    - [x] Opacity

- [x] Colors panel 
    - [x] Color selection
    - [x] Pass into brush stroke data 

- [x] Brush panel
    - [x] Panel creation
    - [x] Panel functionality 
        - [x] brush size and spacing

- [x] Tiling
    - [x] arbitrary sized canvas
    - [x] fix mouse + brush size positions
    - [x] general tiling system
        - [x] tile location calculation
    - [x] Move selected layer into the brush stroke action

- [x] Bug: Purple flicker ?
- [x] Bug: Purple flicker in tiling
    - [x] Resovles if entire image is drawn
    - [x] Error is bad at the corners
    - [x] Sometimes, tiles are not rendered, missing brush stamps appear
- [x] Bug: Not drawing at the corners
    - [x] Do this by hand, resolve all the edge cases 
    
    - [x] Bug: Values are not being calculated correctly
- [x] Figure out render pipeline for opacity and colors
    
- [x] Rendering from top down on affected layers
- [x] Layer deletion
- [x] Get layer targets working

- [x] Clean up the brush engine
- [x] Bug: Brush / Layer engine, not drawing at edges
- [x] Bug: Sometimes, drawing between tiles gets clipped
    
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
