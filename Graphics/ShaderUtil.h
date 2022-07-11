/**
    TinySL Shader code format:

    Each shader program may contain a number of sections:

    States
    Share
    Vertex
    Geometry
    TesselationControl
    TesselationEval
    Fragment

    Shader code should be enclosed by brackets behind its section name like:

    Vertex
    {
        ......
    }

    Everything outside braces is ignored except for #inlcude instructions.

    #include is supported so code can be reused easily, it should be at the begining of file, outside of
    any sections. The rule is: Included source code is inserted to current file by section name, and
    the code snippet will be placed in front of corresponding section. File will be included only once.

    There is a special section "Share", code in share section is inserted to every other section of current file.

    Another special section is "States", it describte the pipeline states when using current shader, like
    Depth test on/off, Blend function and so on. States section in included file is ignored.

    States
    {
        // DepthStencilFunc = less|equal|greater|lEqual|gEqual|notEqual|always|nerver
        // BlendFunc = zero|one|srcColor|oneMinusSrcColor|dstColor|oneMinusDstColor|srcAlpha|oneMinusSrcAlpha|dstAlpha|oneMinusDstAlpha
        // StencilOp = keep,zero,replace,increase,increaseWrap,decrease,decreaseWrap,invert

        ZTest           on|off                                              // default on
        ZWrite          on|off                                              // default on
        ZTestFunc       DepthStencilFunc                                    // default less

        Stencil         on|off                                              // default off
        StencilMask     mask(hex or decimal)                                // 0xFFFFFFFF
        StencilFunc     DepthStencilFunc refValue refMask                   // default always,1,0xFFFFFFFF
        StencilOp       (sfail)StencilOp (dfail)StencilOp (dpass)StencilOp  // default keep keep keep

        Blend           on|off                                              // default off
        BlendEquation   add|subtract|reverseSubtract                        // default add
        BlendFunc       (src)BlendFunc (dst)BlendFunc                       // default srcAlpha oneMinusSrcAlpha

        Cull            on|off                                              // default on
        CullFace        front|back|both                                     // default back
    }
*/

#pragma once

#include "string"
#include "ShaderProgram.h"

namespace Graphics
{
    class ShaderUtil
    {
    public:
        static ShaderProgram::SP LoadProgramFromRaw(const std::string &vertFile, const std::string &fragFile);
        static ShaderProgram::SP LoadProgramFromTinySL(const std::string &vertFile);

        // defaut is #version 460 core
        static void SetTinySLVersionString(const std::string &version);
    };
}