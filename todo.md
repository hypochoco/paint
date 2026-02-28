
# Current development notes

Painting application:

- [ ] Bug: Purple flicker ?
- [ ] Bug: Purple flicker in tiling
    - [ ] Resovles if entire image is drawn
    - [ ] Error is bad at the corners
    - [ ] Sometimes, tiles are not rendered, missing brush stamps appear
- [ ] Bug: Not drawing at the corners
    - [ ] Do this by hand, resolve all the edge cases 
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


Synchronization issues
The most common cause is a semaphore or fence not being properly waited on before you reuse a resource. If frame N+3 starts rendering before frame N has finished presenting, you can end up sampling from a swapchain image or attachment that's still being written or is in an undefined layout. Double-check that your per-frame fences are actually being waited on before you record and submit that frame's command buffer.
Image layout transitions
If an image is in UNDEFINED layout when you sample it — either because a transition was missed or happened out of order — the driver reads garbage. Some drivers/GPUs return bright magenta for undefined image reads rather than black. Check your pipeline barriers and renderpass initialLayout/finalLayout settings carefully, especially for your color attachments.
Descriptor set / resource aliasing
With multiple frames in flight you need separate per-frame descriptor sets (or at least separate buffers/images per frame). If two frames in flight share a descriptor set and one updates it while the other is still using it, you'll get corruption. This is a subtle one because it's not always a GPU sync issue — even a CPU-side update to a descriptor that the GPU is still consuming causes this.
Command buffer reuse too early
If you reset/re-record a command buffer before the GPU has finished executing it, the commands mid-execution can become garbage. Make sure your fence-wait happens before vkResetCommandPool or vkBeginCommandBuffer.
Validation layers
If you haven't already, run with Vulkan validation layers enabled (VK_LAYER_KHRONOS_validation). Sync errors especially will be caught and logged. Also consider enabling VK_EXT_debug_utils so you can name your objects and get more readable validation output. The intermittent nature of the bug is a strong sign it's timing-dependent, and validation layers will often catch the exact frame it happens on.
The intermittent-but-reproducible nature strongly points to a fence/semaphore issue or image layout race rather than something structural like a missing descriptor binding (which would be consistent every frame). I'd start by auditing your fence waits and image layout transitions.
```

```
The fundamental issue is that you want the main thread to race ahead preparing the next frame's data while the worker processes the current one, but you have shared mutable state (the canvas texture, the current frame index) that makes this unsafe.
Here's the proper mental model for async multi-frame rendering:
Frame preparation (main thread) and frame execution (render thread) need to be decoupled by one frame. The main thread should always be building frame N+1 while the worker executes frame N. This is a classic producer/consumer pipeline.
The key insight is that advanceFrame() and fence management belong to different concerns:

The fence guards GPU resources (command buffers, semaphores) for a given frame slot
The frame index advance should happen when you commit a frame slot to the worker, not when the worker finishes

So the corrected flow looks like this:
Main thread:
  waitForFences(currentFrame)     // wait for GPU done with this slot
  acquireNextImage()
  buildFrameGraph(currentFrame)
  advanceFrame()                  // advance AFTER acquiring, before queuing
  emit queueFrame(frameGraph)
  // immediately loop back, prepare next frame

Worker thread:
  receive frameGraph
  record command buffer
  emit frameReady(frameGraph)

Main thread (submitFrame):
  queueSubmit()
  queuePresent()
  // no advanceFrame() here
The fence wait at the top of startFrame already correctly gates you — you can't start recording frame slot N again until the GPU is done with it. Moving advanceFrame() to just before emit queueFrame is the right fix.
However, the canvas texture is still a problem. With this setup, frame N+1 could be writing brush stamps to the canvas while frame N's command buffer (still in-flight on the GPU) is reading it for the blit. The fence only protects the command buffer slot, not the canvas image itself.
You have a few options for the canvas:
The simplest is to give up on the single-canvas approach for in-flight frames and use a double-buffered canvas — two copies of the canvas texture, alternating which one gets stamps written to it per frame. Each frame blits from its own canvas copy, and you copy forward the previous canvas into the current one at the start of each frame's stamp pass. This is the most GPU-friendly approach.
A lighter alternative is to keep one canvas but use a semaphore or explicit barrier to ensure the stamp render pass fully completes before the blit begins. Since both happen in the same command buffer this is already handled within a frame, but you need to ensure the previous frame's submission has fully signaled before you start writing stamps in the new frame. You can do this by having the worker wait on the previous frame's render-finished semaphore before beginning the stamp pass — essentially chaining frame submissions.
The cleanest production approach is to treat the canvas as a persistent resource protected by its own timeline semaphore (if you have Vulkan 1.2+). The stamp pass signals it at a value, the blit waits on it, and the next frame's stamp pass waits on the blit's completion value before writing again. This gives you precise ordering without stalling the CPU.
For your use case, the double-buffered canvas is probably the right tradeoff — it's straightforward to implement, avoids any cross-frame GPU hazards, and the memory cost of one extra canvas texture is usually acceptable. The timeline semaphore approach is worth it if you later need more than two frames of parallelism or have tighter memory constraints.
```

---

- [ ] Bug: Brushes on different layers are not merging correctly

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

- [x] Clean up the brush engine
- [x] Bug: Brush / Layer engine, not drawing at edges
- [x] Bug: Sometimes, drawing between tiles gets clipped

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
