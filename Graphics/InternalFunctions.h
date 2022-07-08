// this header is used in render_system module internally, should only be included in .cpp

#pragma once

#include "GL/glew.h"
#include "Constants.h"

namespace RenderSystem
{
    static uint32_t BufferUsage2Native[BufferUsage_Max] = {GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW};
    static uint32_t BufferType2Native[BufferType_Max] = {GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER, GL_UNIFORM_BUFFER, GL_SHADER_STORAGE_BUFFER, GL_ATOMIC_COUNTER_BUFFER};
    static uint32_t DrawType2Native[DrawType_Max] = {GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_LINES, GL_LINE_STRIP};
    static uint32_t DepthStencilFunc2Native[DepthStencilFunc_Max] = {GL_ALWAYS, GL_NEVER, GL_LESS, GL_GREATER, GL_EQUAL, GL_NOTEQUAL, GL_LEQUAL, GL_GEQUAL};
    static uint32_t StencilOp2Native[StencilOp_Max] = {GL_KEEP, GL_ZERO, GL_REPLACE, GL_INCR, GL_INCR_WRAP, GL_DECR, GL_DECR_WRAP, GL_INVERT};
    static uint32_t BlendFunc2Native[BlendFunc_Max] = {GL_ZERO, GL_ONE, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, \
                                                        GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_CONSTANT_COLOR, \
                                                        GL_ONE_MINUS_CONSTANT_COLOR, GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA};
    static uint32_t BlendEquation2Native[BlendEquation_Max] = {GL_FUNC_ADD, GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT};
    static uint32_t CullFace2Native[CullFace_Max] = {GL_FRONT, GL_BACK, GL_FRONT_AND_BACK};

    inline uint32_t GetNativeBufferType(BufferType t)
    {
        return BufferType2Native[t];
    }

    inline uint32_t GetNativeBufferUsage(BufferUsage usage)
    {
        return BufferUsage2Native[usage];
    }

    inline uint32_t GetNativeDrawType(DrawType usage)
    {
        return DrawType2Native[usage];
    }

    inline uint32_t GetNativeDepthStencilFunc(DepthStencilFunc func)
    {
        return DepthStencilFunc2Native[func];
    }

    inline uint32_t GetNativeStencilOp(StencilOp op)
    {
        return StencilOp2Native[op];
    }

    inline uint32_t GetNativeBlendFunc(BlendFunc func)
    {
        return BlendFunc2Native[func];
    }

    inline uint32_t GetNativeBlendEquation(BlendEquation eq)
    {
        return BlendEquation2Native[eq];
    }

    inline uint32_t GetNativeCullFace(CullFace face)
    {
        return CullFace2Native[face];
    }

    inline ProgramDataType GLType2ProgramDataType(int GLType)
    {
        switch (GLType)
        {
        case GL_FLOAT:
            return ProgramDataType_Float;
        case GL_FLOAT_VEC2:
            return ProgramDataType_Vec2;
        case GL_FLOAT_VEC3:
            return ProgramDataType_Vec3;
        case GL_FLOAT_VEC4:
            return ProgramDataType_Vec4;
        case GL_INT:
            return ProgramDataType_Int;
        case GL_FLOAT_MAT4:
            return ProgramDataType_Mat4;
        }

        return ProgramDataType_None;
    }

#ifdef NDEBUG
#define glCheckError()
#else
#define glCheckError()                                        \
    {                                                         \
        GLenum errorCode;                                     \
        while ((errorCode = glGetError()) != GL_NO_ERROR)     \
        {                                                     \
            const char *error = "";                           \
            switch (errorCode)                                \
            {                                                 \
            case GL_INVALID_ENUM:                             \
                error = "INVALID_ENUM";                       \
                break;                                        \
            case GL_INVALID_VALUE:                            \
                error = "INVALID_VALUE";                      \
                break;                                        \
            case GL_INVALID_OPERATION:                        \
                error = "INVALID_OPERATION";                  \
                break;                                        \
            case GL_STACK_OVERFLOW:                           \
                error = "STACK_OVERFLOW";                     \
                break;                                        \
            case GL_STACK_UNDERFLOW:                          \
                error = "STACK_UNDERFLOW";                    \
                break;                                        \
            case GL_OUT_OF_MEMORY:                            \
                error = "OUT_OF_MEMORY";                      \
                break;                                        \
            case GL_INVALID_FRAMEBUFFER_OPERATION:            \
                error = "INVALID_FRAMEBUFFER_OPERATION";      \
                break;                                        \
            }                                                 \
            RS_LOG_ERROR_FMT("GL error detected: %s", error); \
        }                                                     \
    }
#endif
}
