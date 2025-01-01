// Copyright Frost Gorilla, Inc. All Rights Reserved.

#ifndef RENDER_INC_H
#define RENDER_INC_H

#include "core/render_core.h"

/////////////////////////////////////////////////////////
// NOTE(tijani): Decide which backend to use;
// The default rendering method to use is software. If
// Another backend is specified, then use that.
// #if !defined(RENDER_BACKEND) && OS_WINDOWS
// #define RENDER_BACKEND RENDER_BACKEND_SOFTWARE
// #elif
//
// #else
// #endif
//
// #if RENDER_BACKEND == RENDER_BACKEND_SOFTWARE
// #include "swrender/render_sw.h"
// #elif RENDER_BACKEND_D3D11
//
// #elif RENDER_BACKEND_OPENGL
//
// #elif RENDER_BACKEND_VULKAN
//
// #else
// #error Renderer backend not supported
// #endif

#include "software/render_software.h"

#endif // RENDER_INC_H