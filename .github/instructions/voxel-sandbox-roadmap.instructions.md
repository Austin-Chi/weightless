---
description: 'Use when planning or implementing the next steps for this project to become a minecraft-like voxel sandbox game engine. Covers missing subsystems, implementation order, and completion checks.'
---
# Voxel Sandbox Roadmap

Use this instruction when working on the project as a minecraft-like voxel sandbox engine. The current codebase is a solid Metal renderer with GLFW input, a camera, meshes, shaders, and basic resource management, but it is not yet a voxel engine.

## What Is Not Done Yet

The project is still missing the core systems required for a playable voxel sandbox:

- Block and voxel data model
- Chunk storage, lookup, and lifecycle management
- World manager and chunk streaming
- Procedural terrain generation
- Chunk meshing with face culling or greedy meshing
- Voxel-specific rendering pipeline and texture atlas support
- Raycasting for block hit detection
- Block placement and breaking interaction
- Collision-aware player movement and gravity
- Lighting model for skylight and block light
- Save/load persistence for worlds and chunks
- HUD and gameplay UI such as crosshair, hotbar, and selection feedback
- Performance systems such as frustum culling, mesh caching, and background generation

The current renderer draws a hardcoded triangle and quad. That is a useful prototype, but it does not scale to chunked voxel geometry or streaming world content.

## Concrete Next Steps

Follow this order unless there is a strong reason not to:

1. Define the voxel data model.
   - Add block IDs, block metadata, and block property definitions.
   - Decide how voxel coordinates map between local, chunk, and world space.
   - Define chunk dimensions early and document them in the model layer.

2. Build chunk and world containers.
   - Add a chunk type that stores voxel state and dirty flags.
   - Add a world manager that can query, load, unload, and mutate chunks.
   - Keep the world API separate from rendering so gameplay code can query blocks directly.

3. Add terrain generation.
   - Implement a generator that can fill new chunks from noise or height maps.
   - Keep generation deterministic for a given seed.
   - Generate one chunk first, then expand to multiple chunks and streaming.

4. Replace hardcoded mesh drawing with chunk meshing.
   - Build chunk meshes from voxel faces only, not per-voxel cubes.
   - Start with naive face culling, then upgrade to greedy meshing if needed.
   - Cache meshes and rebuild only dirty chunks.

5. Add a voxel render path.
   - Introduce a chunk render pipeline and a texture atlas.
   - Update vertex formats so UVs and any block-visibility data match the new shader contract.
   - Keep the colored triangle and textured quad paths only as debug/prototype paths.

6. Add block interaction.
   - Implement camera raycasting to find the targeted voxel.
   - Add block placement and block breaking.
   - Expose the hit result so UI and gameplay logic can react to it.

7. Add physics and collision.
   - Make player movement collision-aware.
   - Add gravity, step-up or slope handling, and block boundary checks.
   - Keep camera movement separate from raw input so physics can own final motion.

8. Add lighting and visibility rules.
   - Start with simple skylight or unlit rendering if that is faster.
   - Add block light and propagation later if the visual design needs it.
   - Make lighting data chunk-local and rebuild it when blocks change.

9. Add persistence.
   - Serialize chunks and world metadata to disk.
   - Make save/load deterministic and versioned.
   - Validate load paths before the renderer depends on the world state.

10. Add engine polish.
    - Add HUD and hotbar.
    - Add frustum culling, chunk visibility filtering, and mesh reuse.
    - Add background loading if world generation or meshing becomes expensive.

## Development Rules

- Do not add gameplay systems before the world, chunk, and meshing foundation exists.
- Do not expand the renderer with more hardcoded draw calls for voxels.
- Keep Metal resource ownership explicit and release old resources on reassignment.
- Keep shader buffer indices, vertex attributes, and texture bindings synchronized between C++ and Metal code.
- Keep file paths and runtime asset loading resilient to different launch working directories.
- Prefer small, testable slices: one chunk, one mesh builder, one raycast, one interaction path.

## Completion Checks

A step is only done when the following are true:

- The new subsystem has a clear file-level owner.
- The CPU-side data contract matches the Metal shader contract.
- The feature can be exercised with a minimal test case or visible runtime result.
- The code path handles ownership, cleanup, and reload cases without leaks or crashes.
- The next layer of the engine can use the subsystem without adding temporary hacks.

## Suggested Working Order For The Coding Agent

When asked to continue building this project, start by asking: what is the smallest world feature that can render and be interacted with end to end?

A good default sequence is:

- one chunk in memory
- one generated terrain sample
- one chunk mesh on screen
- one raycast hit on a block
- one block place or break action
- one collision-aware player movement rule
- one save/load round trip

If a task tries to jump past this order, stop and re-anchor on the first missing foundation layer.
