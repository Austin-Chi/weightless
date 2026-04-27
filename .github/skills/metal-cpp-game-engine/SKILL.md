---
name: metal-cpp-game-engine
description: 'Use when working on C++ Metal game-engine code, metal-cpp wrappers, render pipelines, mesh/vertex formats, shader loading, CAMetalLayer integration, GLFW windowing, camera movement, resource lifetime, or CMake setup in this workspace.'
argument-hint: 'engine task, shader, pipeline, mesh, camera, render loop, or build issue'
---

# Metal C++ Game Engine Skill

Use this skill when implementing or debugging a Metal-based game engine written in C++ and Objective-C++ with metal-cpp. It combines three layers of knowledge: metal-cpp ownership and API usage, game-engine architecture and update flow, and the specific file layout and rendering contracts in this repository.

metal-cpp is a header-only C++ interface over Metal with direct class and enum mappings, no wrapper container overhead, and Cocoa-style ownership rules. It is designed for engine code that wants to stay in C++ while calling Metal directly from render, asset, and platform layers.

## What to Inspect First

Start from the owning code path for the behavior you want to change:

- [Metal_Engine_cpp/src/main.cpp](../../Metal_Engine_cpp/src/main.cpp) for application entry.
- [Metal_Engine_cpp/src/control/app.cpp](../../Metal_Engine_cpp/src/control/app.cpp) and [app.h](../../Metal_Engine_cpp/src/control/app.h) for window creation, input, and the main render loop.
- [Metal_Engine_cpp/src/view/renderer.cpp](../../Metal_Engine_cpp/src/view/renderer.cpp) and [renderer.h](../../Metal_Engine_cpp/src/view/renderer.h) for command buffers, render pass setup, depth state, and draw submission.
- [Metal_Engine_cpp/src/view/pipeline_factory.cpp](../../Metal_Engine_cpp/src/view/pipeline_factory.cpp) and [pipeline_factory.h](../../Metal_Engine_cpp/src/view/pipeline_factory.h) for shader loading and pipeline compilation.
- [Metal_Engine_cpp/src/view/mesh.cpp](../../Metal_Engine_cpp/src/view/mesh.cpp), [mesh.h](../../Metal_Engine_cpp/src/view/mesh.h), [mesh_factory.cpp](../../Metal_Engine_cpp/src/view/mesh_factory.cpp), and [mesh_factory.h](../../Metal_Engine_cpp/src/view/mesh_factory.h) for vertex/index/texture/sampler ownership.
- [Metal_Engine_cpp/src/view/vertex_formats.cpp](../../Metal_Engine_cpp/src/view/vertex_formats.cpp) and [vertex_formats.h](../../Metal_Engine_cpp/src/view/vertex_formats.h) for vertex descriptors and attribute layouts.
- [Metal_Engine_cpp/src/model/camera.cpp](../../Metal_Engine_cpp/src/model/camera.cpp) and [camera.h](../../Metal_Engine_cpp/src/model/camera.h) for first-person movement and view matrices.
- [Metal_Engine_cpp/src/backend/glfw_adapter.mm](../../Metal_Engine_cpp/src/backend/glfw_adapter.mm) and [glfw_adapter.h](../../Metal_Engine_cpp/src/backend/glfw_adapter.h) for GLFW-to-AppKit window bridging.
- [Metal_Engine_cpp/src/backend/mtlm.cpp](../../Metal_Engine_cpp/src/backend/mtlm.cpp) and [mtlm.h](../../Metal_Engine_cpp/src/backend/mtlm.h) for matrix math utilities.
- [Metal_Engine_cpp/src/shaders/triangle.metal](../../Metal_Engine_cpp/src/shaders/triangle.metal) and [general.metal](../../Metal_Engine_cpp/src/shaders/general.metal) for shader entry points and buffer slot contracts.
- [CMakeLists.txt](../../CMakeLists.txt) for framework linking, Objective-C++ flags, and include paths.

## metal-cpp Fundamentals

Use these rules whenever the task touches Metal objects or platform integration:

- Ownership follows Cocoa conventions. Objects created with `alloc`, `new`, `copy`, `mutableCopy`, or `Create` are owned by the caller.
- If you store a Metal object on an engine class, retain it on assignment and release the previous value first.
- When ownership is temporary, prefer an `AutoreleasePool` around the smallest sensible scope, usually a frame or thread entry point.
- `NS::SharedPtr` is optional, but useful when an engine subsystem wants RAII-style lifetime management.
- `MTL::Device::supports...()` helpers already account for selector availability, so feature checks are safer than raw assumptions.
- `NS_PRIVATE_IMPLEMENTATION`, `MTL_PRIVATE_IMPLEMENTATION`, and `CA_PRIVATE_IMPLEMENTATION` are required in exactly one translation unit when you need symbol linkage for the metal-cpp wrappers.
- `metal-cpp` is intended to be lightweight and direct; prefer using the native Metal type that maps to the API you need instead of inventing local wrappers.

## Game Engine Principles

When solving engine tasks, think in terms of pipeline stages and ownership boundaries:

- Platform layer: window creation, message pump, input, and OS-specific integration.
- Core loop: frame pacing, update, render submission, and shutdown.
- Scene/model layer: camera, transforms, entities, and high-level gameplay state.
- Render layer: shader binding, mesh submission, materials, textures, and render passes.
- Asset layer: file paths, shader compilation, texture loading, and mesh generation.
- Memory layer: object ownership, lifetime boundaries, and resource cleanup.

Prefer changes that preserve those boundaries. If a fix crosses layers, step through them in order rather than patching the symptom in the wrong subsystem.

## Core Workflow

1. Identify the smallest engine layer that controls the behavior.
   - App startup and input belong in `control/`.
   - Metal object creation and submission belong in `view/`.
   - Math and transforms belong in `backend/` and `model/`.
   - Shader interface mismatches belong in `src/shaders/` and vertex format code.

2. Verify the data contract across CPU and GPU code.
   - Check that vertex attributes, buffer indices, and texture/sampler slots match between C++ and Metal shader code.
   - Check that pipeline pixel formats match the `CAMetalLayer` and depth attachment formats.
   - Check that the camera matrix order matches the shader multiplication order.

3. Preserve Metal resource lifetime rules.
   - Retain objects when storing them on engine classes.
   - Release or null out GPU resources on reassignment and destruction.
   - Prefer a single owner per resource path unless the object is intentionally shared.

4. Validate shader loading and pipeline creation before debugging draw logic.
   - Confirm the shader file exists at the path the process uses at runtime.
   - Confirm the entry point names in C++ match the Metal function names exactly.
   - Confirm the vertex descriptor matches the shader attribute layout.

5. Rebuild and run the narrowest relevant slice.
   - If the issue is a shader or pipeline change, validate the pipeline builder and renderer first.
   - If the issue is camera motion, validate `Camera` and the view matrix before touching rendering.
   - If the issue is a resource leak, inspect mesh ownership and renderer teardown.

6. Use engine-friendly debug heuristics.
   - If the frame is blank, verify the clear color, drawable acquisition, pipeline state, and camera transform before changing gameplay logic.
   - If geometry appears skewed or missing, verify vertex stride, attribute indices, and matrix multiplication order.
   - If textures fail, verify image loading, sampler configuration, and texture binding slots.
   - If input feels wrong, verify cursor capture, delta computation, and camera basis updates.

## Engine Conventions in This Repository

- `App` owns GLFW initialization, the Metal device, the `CAMetalLayer`, the renderer, and the camera.
- `Renderer` owns the command queue, pipelines, depth state, depth texture, and the draw sequence.
- `Mesh` owns the Metal buffers, texture, and sampler used for a draw call.
- `PipelineBuilder` loads shader source from disk and builds a `MTL::RenderPipelineState`.
- `Vertex` and `TexturedVertex` define the attribute layout for colored and textured geometry.
- `Camera` provides first-person motion and returns the view transform consumed by the renderer.
- `triangle.metal` and `general.metal` define the shader-side contract for colored and textured draws.

## Important Code References

Use these as the primary anchors when extending the engine:

- `main.cpp` for the application entry point and autorelease-pool lifetime.
- `app.cpp` for GLFW input, cursor recentering, and frame-by-frame camera updates.
- `renderer.cpp` for `nextDrawable()`, `RenderPassDescriptor`, depth texture resizing, and draw ordering.
- `pipeline_factory.cpp` for `newLibrary`, `newFunction`, and render pipeline creation.
- `mesh_factory.cpp` for triangle and quad construction, texture loading, and sampler configuration.
- `mesh.cpp` for draw binding order and release behavior.
- `vertex_formats.cpp` for vertex descriptor construction.
- `camera.cpp` for camera basis vectors and matrix generation.
- `triangle.metal` for the simple colored pipeline contract.
- `general.metal` for textured rendering contracts.
- `CMakeLists.txt` for `-fno-objc-arc`, framework linking, and source registration.

## Metal-Cpp and Engine Knowledge to Apply

### API and platform usage

- Create the Metal device once at startup with `MTL::CreateSystemDefaultDevice()` and pass it down to systems that need it.
- Keep `CAMetalLayer` pixel format, render-pipeline color format, and drawable setup aligned.
- Use `NS::String` and `NS::Error` for compile and pipeline diagnostics, and always print error descriptions when available.
- Prefer `MTL::RenderPassDescriptor`, `MTL::RenderPipelineDescriptor`, `MTL::DepthStencilDescriptor`, and `MTL::TextureDescriptor` over ad hoc wrappers.

### Render loop design

- Acquire a drawable as late as possible in the frame.
- Build the render pass descriptor from the drawable and current depth texture size.
- Encode render commands in one clear order: set pipeline, set depth state, bind uniform buffers, bind mesh resources, draw.
- Present and commit after encoding is complete.

### Asset and content pipeline

- Keep shader entry points explicit and stable.
- Keep vertex descriptors in sync with shader attributes and mesh data layout.
- Treat asset paths as runtime inputs, not compile-time facts.
- If the engine grows, move file loading and asset validation into dedicated systems instead of scattering them across render code.

### Game-engine scaling guidance

- If the project adds more renderable objects, introduce a scene or renderable list rather than hard-coding more draw calls in the renderer.
- If materials multiply, split mesh geometry from material state.
- If the game loop expands, separate update, input, simulation, and render submission into distinct stages.
- If you add background loading, give each worker thread its own autorelease pool.

## Common Fix Patterns

### Shader or pipeline fails to build

- Check the shader path first.
- Confirm the runtime working directory matches the path passed to the pipeline builder.
- Confirm the entry point strings are the same in both C++ and Metal.
- Confirm the vertex descriptor matches the shader attributes.

### Frame renders black or nothing appears

- Check `nextDrawable()` is returning a drawable.
- Check the color pixel format on the layer and pipeline.
- Check depth texture creation and render-pass attachment setup.
- Check that the camera and model matrices are being sent to the expected buffer slots.

### Textures are missing or incorrect

- Check that the mesh set a texture and sampler before drawing.
- Check fragment shader texture and sampler slots.
- Check that the texture loading path resolves relative to the runtime working directory.

### Memory leaks or crashes during teardown

- Check that every retained Metal object is released exactly once.
- Check that reassignment paths release the previous resource before retaining the new one.
- Check destructor ordering for renderer, window, and device ownership.

### Extending the engine safely

- Add new geometry through `mesh_factory.cpp` first, then wire it into `renderer.cpp`.
- Add new shader behavior by updating the Metal file and the matching vertex descriptor or buffer bindings together.
- Add new camera behavior in `camera.cpp` and validate the shader matrix order still matches.
- Add new Metal resources with explicit ownership and teardown paths before increasing draw complexity.

## Metal-Specific Contracts to Remember

- `triangle.metal` expects `transform` at buffer 1, `projection` at buffer 2, and `view` at buffer 3.
- `general.metal` uses the same buffer contract and adds texture and sampler bindings at slot 0.
- `renderer.cpp` currently binds the textured mesh to `generalPipeline` and the simple triangle to `trianglePipeline`.
- `renderer.cpp` rebuilds the depth texture when drawable size changes, so depth and color attachments stay aligned.
- `CMakeLists.txt` disables ARC for OBJCXX sources, which is required for the metal-cpp usage pattern here.

## Cross-Reference Notes

For broader metal-cpp guidance, consult the upstream overview in [metal-cpp/README.md](../../metal-cpp/README.md). Use it for ownership rules, `NS::SharedPtr`, autorelease pools, and symbol-implementation macros when you need deeper API behavior than this project alone shows.

## Suggested Validation Loop

1. Make the smallest local change in the owning file.
2. Re-check the linked contract in the paired CPU/GPU file.
3. Build the project.
4. Run the app and verify either the shader path, the pipeline state, the camera motion, or the draw output.
5. If the failure persists, step one layer down or up in the chain, not sideways.
