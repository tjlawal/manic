// Copyright Frost Gorilla, Inc. All Rights Reserved.

#ifndef RENDER_CORE_H
#define RENDER_CORE_H

// tijani: Render Modes

#define RENDER_DEFAULT   (1 << 0)
#define RENDER_WIREFRAME (1 << 1)
#define RENDER_VERTEX    (1 << 2)
#define RENDER_FILL      (1 << 3)

// tijani: Culling Modes

#define CULL_NONE  (1 << 0)
#define CULL_BACK  (1 << 1)
#define CULL_FRONT (1 << 2)

// tijani: Render State
typedef struct RenderState RenderState;
struct RenderState {
  u32 cull_mode;
  u32 render_mode;
  b32 debug_overlay;
};

#endif // RENDER_CORE_H