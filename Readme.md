Practice C++ and graphics programming.

## Plan

This project used to be using OpenGL 4.6, but now I'm writing code on an old macbook pro, which supports only 4.1. So I decide to migrate everything to modern graphics API like vulkan, DX12, metal. Here are some key plans:

* Cross compile shaders, use HLSL instead glsl as the shading language, use DXC to compile HLSL to DXIL or SPIR-V, then use SPIRV-CROSS to compile it to metal sl.
* Using modern Graphics API, firstly supports metal, then vulkan, lastly DX12.
* Discard evertying related to resource bindings, use bindless descriptors.

After finish these fundation refactoring, I can move on other things like:

* Common scene format support like gltf, usd and so on.
* Multi-threaded rendering.
* So many other features to do ......