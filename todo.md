
# Current development notes

Painting application:

- [ ] Bug: Purple flicker ?
- [ ] Bug: Culling distance on zoom

```
Purple/magenta flicker is a classic sign of an uninitialized or cleared render target. A few common causes:
Clear color — check your render pass clear values. If you have a clear color set to something like {1, 0, 1, 1} (magenta) for debugging purposes that's your culprit. It's a common convention to use magenta as a "this shouldn't be visible" clear color.
Pipeline barrier / layout transition timing — on MoltenVK specifically, image layout transitions can sometimes expose a frame where the image is in an unexpected state, briefly showing whatever was in memory.
Swapchain image not being written — if a frame slips through where your render pass clears but doesn't complete drawing, you'd see the clear color for one frame.
Descriptor set pointing to wrong image — if a descriptor is updated while in use, you might briefly sample garbage.
I'd start by checking your clear color values first — magenta specifically is almost always an intentional debug clear color that someone set and forgot about.there are no magenta in colors in my code at least. it may be in the vulkan code itself or something3:56 AMOn MoltenVK, a common cause is that an image is being sampled before its layout transition is complete — MoltenVK translates Vulkan barriers to Metal and the timing can be slightly different than on native Vulkan drivers, sometimes exposing race conditions that would be hidden on desktop GPUs.
A few things to check:
Pipeline barriers — make sure every image has a proper layout transition before it's sampled. Missing or incorrect srcStageMask/dstStageMask flags are a common culprit.
Render pass loadOp — if any attachment has VK_ATTACHMENT_LOAD_OP_DONT_CARE and the image wasn't written last frame, you'll get undefined contents which on MoltenVK can show up as colored garbage.
Semaphore/fence synchronization — if you're submitting work without properly waiting on the previous frame, you might be rendering to an image that's still being presented.
MoltenVK validation — try enabling the Metal validation layer in Xcode by going to Edit Scheme → Run → Diagnostics → Metal → Enable API Validation. This can catch synchronization issues that Vulkan validation misses on MoltenVK.
The fact that it's only a flicker suggests it's a synchronization issue rather than a consistent bug — something is occasionally racing. The Metal API validation layer is probably your best next step since it operates at a lower level than Vulkan validation on Mac.
```

---

- [x] Clean up the brush engine
- [x] Bug: Brush / Layer engine, not drawing at edges
- [x] Bug: Sometimes, drawing between tiles gets clipped

- [ ] Bug: Brushes on different layers are not merging correctly
- [ ] Is color selection off ? could we draw the sv square a little darker ?
- [ ] Bug: Purple flicker is more prominent after tiled layer engine 

--- 

- [x] More brush features
    - [x] Colors
    - [x] Opacity
- [ ] Color picker optimization 
- [ ] Handle multiple canvas windows
    - [ ] Dockable canvas widget 
    - [ ] Overhaul of widget structure and general styling
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

- [x] figure out some basic styling
- [ ] complete styling

- [x] Bug: Values are not being calculated correctly
- [x] Figure out render pipeline for opacity and colors
    
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
