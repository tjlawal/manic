# Manic 3D Renderer

Manic is a native 3D renderer. It currently supports rendering 3D meshes on Window x64 with plans to support Linux x64
and MacOS. Manic is currently in ALPHA. This means that it is neither performant 
(i.e. very limited in the amount of triangles it can draw on the screen), nor does it support a wide range of 
file format (currently OBJ) used to store 3D meshes.

## Development Setup Instructions
> [!NOTE]
> Only x64 is supported. While support for x86 is not specifically outlawed, it may not work as the codebase makes 
> specific assumptions on where it would be ran.
<!-- Build instructions here -->


## Codebase Introduction

The codebase is seperated into *layers*. The layers are meant to isolate certain concerns of the codebase . Layers
correspond with folders inside the `src` directory. One folder in the `src` directory will include multiple sub-layers,
but their structure are mostly flat and straight forward.

Layers correspond 1-to-1 with *namespaces*. The term "namespace" in this context does not refere to language specific 
features, but a naming convention for C-style namespaces written as short prefix, usually 5 characters at most but maybe
more (context and verboseness dependent). These namespaces main purpose is to quickly provide an understanding of what
the code does and what layer it belongs. Some layers do not have a namespace, but most do. Namespaces are either 
capitalized (PascalCase) or lowercase. Enums and types are capitalized (PascalCase), functions, variables and macros
are lowercased. Global variables are appended with a `g_` in front of it.

Layers depend on other layers. To prevent circular dependencies, it is arranged as a directed acyclic graph.

List of layers in the codebase and their associated namespaces:
- `base` (no namespace): Available codebase-wide. Contains strings, math, memory allocators, helper macros, etc. It 
	depends on no other layers in the codebase.
- `os/core` (`os_`): This is an abstraction layer that provides core, non-graphical functionality from the operating 
	system under an abstract API. This layer is implemented per-target operating system.
- `os/gfx` (`os_`): This is an abstarction layer that provides graphical operating system functionality from the 
	operating system under an abstract api. It builds on `os/core`. This layer is implemented per-target operating system.
- `render` (`render_`): This is an abstract API for rendering using either software rasterization or either of the 
	various GPU APIs under a common interface. This currently also implements the drawing API but it would be refactored
	to strictly implement the rendering APIs.








## References & Motivations
+ [RADDebugger](https://github.com/EpicGamesExt/raddebugger)
+ [Handmade Hero](https://www.youtube.com/playlist?list=PLnuhp3Xd9PYTt6svyQPyRO_AAuMWGxPzU)

