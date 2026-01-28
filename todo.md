
# Current development notes

Painting application:

- [ ] Cleanup camera hardcoded logic
    - [ ] Screen to world function inside the camera ?

- [ ] Setup brush functionality
    - [x] Get stamping back
    - [x] Interpolation
    - [ ] Undo action / image backups 
    
    - [ ] More interpolation methods 
    - [ ] Round brush
    - [ ] Brush opacity
    - [ ] Brush settings UI

- [ ] Tiling
- [ ] Layer visibility 
- [ ] Canvas flipping

- [ ] Tooling system
    - [ ] Brush tool
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
