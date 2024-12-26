// Copyright Frost Gorilla, Inc. All Rights Reserved.

#ifndef RENDER_CORE_H
#define RENDER_CORE_H

// tijani: Render Modes
typedef enum {
  Render_FillWireframes,   // Fill triangles with wireframe
  Render_FillColour,       // Fill triangles with colour
  Render_WireFramesVertex, // Wireframe with vertex of each triangle vertex
  Render_WireFrames,       // Wireframe only

  Render_Count
} RenderMode;

// tijani: Culling Modes
typedef enum {
  Cull_None,     // Disable all types of culling
  Cull_BackFace, // Enable Back-Face Culling

  Cull_Count
} CullingMode;

// tijani: Render State
typedef struct RenderState RenderState;
struct RenderState {
  b32 is_wireframe_vertex;
  b32 is_wireframes;
  b32 is_fill_colour;
  b32 is_fill_wireframes;
  RenderMode render_mode;
  CullingMode culling_mode;
};

#endif // RENDER_CORE_H