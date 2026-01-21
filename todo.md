
# Current development notes

- [ ] [BUG] Sometimes brush stops stamping
    - [ ] Reiszing appears to update canvas with all the missed changes
    - [ ] Source: low power mode on mac
    - [ ] Force some kind of refresh ?

- [ ] Refactor the render loop
    - [ ] Remove the timer ?
    - [ ] Frame ready
    - [ ] Request frame update ? 
    - [ ] Force some kind of update request for low power mode issue ?
    
    - [ ] Clean up the entire rendering process somehow ... 

- [ ] Move stamping to brush engine 

- [ ] Setup brush functionality
    - [x] Start brush action
    - [x] End brush action
    - [x] Interpolation in between
        - [ ] update so that at least some of the brush stamp can be reused 
    - [x] Brush history
    - [ ] Brush undo button  
    
---

Painting application:

- [ ] Layer visibility 
- [ ] Canvas flipping

- [ ] Tooling system
    - [ ] Brush tool
        - [ ] Brush functions / mouse down and released
    - [ ] Zoom tool
        - [ ] GPU acceleration with zooming in and out ...   
    
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
