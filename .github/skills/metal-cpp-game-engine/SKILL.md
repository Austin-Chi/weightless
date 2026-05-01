---
name: metal-cpp-game-engine
description: 'Use when working on this project''s 3D combat game: metal-cpp wrappers, render pipelines, block-like battlefields, shield and weapon systems, elemental combat rules, zero-gravity movement, camera handling, GLFW windowing, resource lifetime, or CMake setup.'
argument-hint: 'combat, battlefield, shield, weapon, movement, damage, cooldown, or build issue'
---

# Metal C++ 3D Combat Game Skill

Use this skill when implementing or debugging the project's 3D combat game written in C++ and Objective-C++ with metal-cpp. It combines three layers of knowledge: metal-cpp ownership and API usage, game-engine architecture for arena combat, and the specific file layout and rendering contracts in this repository.

metal-cpp is a header-only C++ interface over Metal with direct class and enum mappings, no wrapper container overhead, and Cocoa-style ownership rules. It is designed for engine code that wants to stay in C++ while calling Metal directly from simulation, render, and platform layers.

## What to Inspect First

Start from the owning code path for the behavior you want to change:

- [Metal_Engine_cpp/src/main.cpp](../../Metal_Engine_cpp/src/main.cpp) for application entry.
- [Metal_Engine_cpp/src/control/app.cpp](../../Metal_Engine_cpp/src/control/app.cpp) and [app.h](../../Metal_Engine_cpp/src/control/app.h) for window creation, input, and the main game loop.
- [Metal_Engine_cpp/src/view/renderer.cpp](../../Metal_Engine_cpp/src/view/renderer.cpp) and [renderer.h](../../Metal_Engine_cpp/src/view/renderer.h) for command buffers, render pass setup, depth state, battlefield drawing, and draw submission.
- [Metal_Engine_cpp/src/view/gen/battlefield.cpp](../../Metal_Engine_cpp/src/view/gen/battlefield.cpp) and [battlefield.h](../../Metal_Engine_cpp/src/view/gen/battlefield.h) for battlefield generation and block-instance data.
- [Metal_Engine_cpp/src/view/pipeline_factory.cpp](../../Metal_Engine_cpp/src/view/pipeline_factory.cpp) and [pipeline_factory.h](../../Metal_Engine_cpp/src/view/pipeline_factory.h) for shader loading and pipeline compilation.
- [Metal_Engine_cpp/src/view/mesh.cpp](../../Metal_Engine_cpp/src/view/mesh.cpp), [mesh.h](../../Metal_Engine_cpp/src/view/mesh.h), [mesh_factory.cpp](../../Metal_Engine_cpp/src/view/mesh_factory.cpp), and [mesh_factory.h](../../Metal_Engine_cpp/src/view/mesh_factory.h) for vertex/index/texture/sampler ownership and prototype block geometry.
- [Metal_Engine_cpp/src/view/vertex_formats.cpp](../../Metal_Engine_cpp/src/view/vertex_formats.cpp) and [vertex_formats.h](../../Metal_Engine_cpp/src/view/vertex_formats.h) for vertex descriptors and attribute layouts.
- [Metal_Engine_cpp/src/model/camera.cpp](../../Metal_Engine_cpp/src/model/camera.cpp) and [camera.h](../../Metal_Engine_cpp/src/model/camera.h) for movement, facing, and view matrices.
- [Metal_Engine_cpp/src/backend/glfw_adapter.mm](../../Metal_Engine_cpp/src/backend/glfw_adapter.mm) and [glfw_adapter.h](../../Metal_Engine_cpp/src/backend/glfw_adapter.h) for GLFW-to-AppKit window bridging.
- [Metal_Engine_cpp/src/backend/mtlm.cpp](../../Metal_Engine_cpp/src/backend/mtlm.cpp) and [mtlm.h](../../Metal_Engine_cpp/src/backend/mtlm.h) for matrix math utilities.
- [Metal_Engine_cpp/src/shaders/triangle.metal](../../Metal_Engine_cpp/src/shaders/triangle.metal) and [general.metal](../../Metal_Engine_cpp/src/shaders/general.metal) for shader entry points and buffer slot contracts.
- [CMakeLists.txt](../../CMakeLists.txt) for framework linking, Objective-C++ flags, and include paths.

## metal-cpp Fundamentals

Use these rules whenever the task touches Metal objects or platform integration:

- Ownership follows Cocoa conventions. Objects created with `alloc`, `new`, `copy`, `mutableCopy`, or `Create` are owned by the caller.
- If you store a Metal object on an engine class, retain it on assignment and release the previous value first.
- When ownership is temporary, prefer an `AutoreleasePool` around the smallest sensible scope, usually a frame or thread entry point.
- `NS::SharedPtr` is optional, but useful when a subsystem wants RAII-style lifetime management.
- `MTL::Device::supports...()` helpers already account for selector availability, so feature checks are safer than raw assumptions.
- `NS_PRIVATE_IMPLEMENTATION`, `MTL_PRIVATE_IMPLEMENTATION`, and `CA_PRIVATE_IMPLEMENTATION` are required in exactly one translation unit when you need symbol linkage for the metal-cpp wrappers.
- `metal-cpp` is intended to be lightweight and direct; prefer using the native Metal type that maps to the API you need instead of inventing local wrappers.

## Game Engine Principles

When solving engine tasks, think in terms of pipeline stages and ownership boundaries:

- Platform layer: window creation, message pump, input, and OS-specific integration.
- Core loop: frame pacing, update, render submission, and shutdown.
- Arena simulation layer: battlefield generation, block occupancy, spawn logic, and encounter triggering.
- Combat layer: player stats, shield and weapon attributes, cooldowns, elemental rules, turn resolution, and win/loss state.
- Render layer: shader binding, mesh submission, materials, textures, and render passes.
- Asset layer: file paths, shader compilation, texture loading, and mesh generation.
- Memory layer: object ownership, lifetime boundaries, and resource cleanup.

Prefer changes that preserve those boundaries. If a fix crosses layers, step through them in order rather than patching the symptom in the wrong subsystem.

## Core Workflow

1. Identify the smallest engine layer that controls the behavior.
   - App startup and input belong in `control/`.
   - Battlefield generation belongs in `view/gen/`.
   - Metal object creation and submission belong in `view/`.
   - Math and transforms belong in `backend/` and `model/`.
   - Shader interface mismatches belong in `src/shaders/` and vertex format code.

2. Verify the data contract across CPU, combat logic, and GPU code.
   - Check that vertex attributes, buffer indices, and texture/sampler slots match between C++ and Metal shader code.
   - Check that battlefield instance data, arena dimensions, and spawn positions match the render path.
   - Check that shield, weapon, cooldown, HP, and elemental rules are encoded in one shared combat model instead of being duplicated in UI or rendering.
   - Check that the camera matrix order matches the shader multiplication order.

3. Preserve Metal resource lifetime rules.
   - Retain objects when storing them on engine classes.
   - Release or null out GPU resources on reassignment and destruction.
   - Prefer a single owner per resource path unless the object is intentionally shared.

4. Validate arena generation and pipeline creation before debugging combat logic.
   - Confirm the battlefield generation matches the expected block-like arena shape.
   - Confirm the shader file exists at the path the process uses at runtime.
   - Confirm the entry point names in C++ match the Metal function names exactly.
   - Confirm the vertex descriptor matches the shader attribute layout.

5. Rebuild and run the narrowest relevant slice.
   - If the issue is battlefield generation or instance rendering, validate `Battlefield` and `Renderer` first.
   - If the issue is movement, validate `Camera` and zero-gravity controls before touching combat.
   - If the issue is combat state, validate shield/weapon data and encounter triggering before changing rendering.
   - If the issue is a resource leak, inspect mesh ownership and renderer teardown.

6. Use engine-friendly debug heuristics.
   - If the frame is blank, verify the clear color, drawable acquisition, pipeline state, and camera transform before changing gameplay logic.
   - If the battlefield shape is wrong, verify generation bounds, instance transforms, and block ownership.
   - If movement feels wrong, verify zero-gravity stepping, wall-jump or ceiling-transfer logic, and current-ground selection.
   - If combat does not trigger, verify the distance threshold, collision check, and battle pause state.
   - If damage feels wrong, verify the formula attack damage * attribute - shield effect and the elemental matchup table.

## Engine Conventions in This Repository

- `App` owns GLFW initialization, the Metal device, the `CAMetalLayer`, the renderer, and the camera.
- `Renderer` owns the command queue, pipelines, depth state, depth texture, and the draw sequence.
- `Battlefield` currently generates a block-like arena shell and uploads instance data to the mesh.
- `Mesh` owns the Metal buffers, texture, and sampler used for a draw call.
- `PipelineBuilder` loads shader source from disk and builds a `MTL::RenderPipelineState`.
- `Vertex` and `TexturedVertex` define the attribute layout for colored and textured geometry.
- `Camera` provides movement and returns the view transform consumed by the renderer.
- `triangle.metal` and `general.metal` define the shader-side contract for prototype draws.

## Important Code References

Use these as the primary anchors when extending the game:

- `main.cpp` for the application entry point and autorelease-pool lifetime.
- `app.cpp` for GLFW input, cursor recentering, and frame-by-frame camera updates.
- `renderer.cpp` for `nextDrawable()`, `RenderPassDescriptor`, depth texture resizing, and draw ordering.
- `gen/battlefield.cpp` for arena generation and block-instance data.
- `pipeline_factory.cpp` for `newLibrary`, `newFunction`, and render pipeline creation.
- `mesh_factory.cpp` for prototype block geometry, textures, and samplers.
- `mesh.cpp` for draw binding order and release behavior.
- `vertex_formats.cpp` for vertex descriptor construction.
- `camera.cpp` for camera basis vectors and matrix generation.
- `triangle.metal` for the simple prototype pipeline contract.
- `general.metal` for textured rendering contracts.
- `CMakeLists.txt` for `-fno-objc-arc`, framework linking, and source registration.

## Metal-Cpp and Game-Engine Knowledge to Apply

### API and platform usage

- Create the Metal device once at startup with `MTL::CreateSystemDefaultDevice()` and pass it down to systems that need it.
- Keep `CAMetalLayer` pixel format, render-pipeline color format, and drawable setup aligned.
- Use `NS::String` and `NS::Error` for compile and pipeline diagnostics, and always print error descriptions when available.
- Prefer `MTL::RenderPassDescriptor`, `MTL::RenderPipelineDescriptor`, `MTL::DepthStencilDescriptor`, and `MTL::TextureDescriptor` over ad hoc wrappers.

### Render loop design

- Acquire a drawable as late as possible in the frame.
- Build the render pass descriptor from the drawable and current depth texture size.
- Encode render commands in one clear order: set pipeline, set depth state, bind uniform buffers, bind arena resources, draw.
- Present and commit after encoding is complete.

### Arena and combat simulation

- Keep battlefield generation deterministic so players can retry the same map with the same seed.
- Separate the visual arena shell from the logical gameplay grid if later gameplay needs more complex collision or traversal rules.
- Make shield and weapon attributes data-driven so elemental strength, weakness, resistance, and cooldowns can be changed without rewriting combat flow.
- Treat player collision as a battle trigger, not just a physics event.
- Model the paused combat round as a distinct state: encounter start, action selection, resolution, cooldown update, return to motion.
- Keep HP, shield effect, attack power, and elemental matchup rules in one combat resolver so the same numbers drive UI, gameplay, and tests.

### Asset and content pipeline

- Keep shader entry points explicit and stable.
- Keep vertex descriptors in sync with shader attributes and mesh data layout.
- Treat asset paths as runtime inputs, not compile-time facts.
- If the game grows, move file loading and asset validation into dedicated systems instead of scattering them across render code.

### Game-engine scaling guidance

- If the project adds more combatants, introduce a combat state or encounter manager rather than hard-coding more branches in the renderer.
- If the arena becomes larger or more complex, split battlefield generation from battlefield meshing.
- If the game loop expands, separate update, input, simulation, battle resolution, and render submission into distinct stages.
- If you add background loading, give each worker thread its own autorelease pool.

## Common Fix Patterns

### Arena generation or instancing fails

- Check the arena dimensions first.
- Confirm the block occupancy mask matches the visible result.
- Confirm instance transforms are uploaded in the same order the mesh expects.
- Confirm the render pipeline and vertex descriptor still match the block mesh layout.

### Movement feels wrong in zero gravity

- Check whether the player is attached to the correct ground block.
- Check wall-jump or ceiling-transfer logic before changing the camera.
- Check that WASD movement is applied in the current local frame, not only world axes.

### Combat does not start or ends too early

- Check the encounter distance threshold and collision test.
- Check the state transition into paused battle mode.
- Check that both players receive the same turn-start state before action selection.

### Damage or elemental rules are incorrect

- Check the shared combat resolver first.
- Confirm attack power, elemental multiplier, and shield reduction are all applied in the intended order.
- Confirm the shield cooldown and weapon cooldown are updated after resolution.

### Memory leaks or crashes during teardown

- Check that every retained Metal object is released exactly once.
- Check that reassignment paths release the previous resource before retaining the new one.
- Check destructor ordering for renderer, window, and device ownership.

### Extending the game safely

- Add new arena generation through `gen/battlefield.cpp` first, then wire it into `renderer.cpp`.
- Add new combat behavior in a dedicated combat model before exposing it to UI or rendering.
- Add new movement rules in `camera.cpp` or a dedicated movement system, not inside the renderer.
- Add new Metal resources with explicit ownership and teardown paths before increasing draw complexity.

## Metal-Specific Contracts to Remember

- Keep buffer slot contracts synchronized between the C++ code and the Metal shader code.
- Keep arena transforms, camera data, and projection data aligned across the render path.
- `renderer.cpp` rebuilds the depth texture when drawable size changes, so depth and color attachments stay aligned.
- `CMakeLists.txt` disables ARC for OBJCXX sources, which is required for the metal-cpp usage pattern here.

## Cross-Reference Notes

For broader metal-cpp guidance, consult the upstream overview in [metal-cpp/README.md](../../metal-cpp/README.md). Use it for ownership rules, `NS::SharedPtr`, autorelease pools, and symbol-implementation macros when you need deeper API behavior than this project alone shows.

## Suggested Validation Loop

1. Make the smallest local change in the owning file.
2. Re-check the linked contract in the paired CPU/GPU file.
3. Build the project.
4. Run the app and verify either the arena shape, movement behavior, battle trigger, combat resolution, or draw output.
5. If the failure persists, step one layer down or up in the chain, not sideways.
