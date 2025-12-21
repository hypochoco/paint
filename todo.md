
# Current development notes

Graphics engine:

- [ ] Onboard onto QT
- [ ] Move camera information into a class
- [ ] More structured / documented API, less manual work with creating and deleting

Painting application:

- [ ] (Bug) Window resizing requires two draw calls to fix, not all frames are updated

- [ ] Tiling with dirty flags 
- [ ] Layer visibility 
- [ ] Configure scissors for better optimization

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
