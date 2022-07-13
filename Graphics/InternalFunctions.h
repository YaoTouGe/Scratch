// this header is used in render_system module internally, should only be included in .cpp

#pragma once

#include "GL/glew.h"
#include "Constants.h"

namespace Graphics
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
    static uint32_t Filter2Native[TextureFilter_Max] = {GL_LINEAR, GL_NEAREST, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR};
    static uint32_t WrapMode2Native[TextureWrapMode_Max] = {GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER, GL_REPEAT, GL_MIRRORED_REPEAT};

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

    inline uint32_t GetNativeWrapMode(TextureWrapMode wrapMode)
    {
        return WrapMode2Native[wrapMode];
    }

    inline uint32_t GetNativeFilter(TextureFilter filter)
    {
        return Filter2Native[filter];
    }

    inline void GetNativeTypeAndFormat(TextureFormat format, uint32_t *nativeType, uint32_t *nativeFormat, int *channel)
    {
        switch (format)
        {
        case TextureFormat_R8G8B8:
            *nativeFormat = GL_RGB;
            *nativeType = GL_UNSIGNED_BYTE;
            *channel = 3;
            break;
        case TextureFormat_R8G8B8A8:
            *nativeFormat = GL_RGBA;
            *nativeType = GL_UNSIGNED_BYTE;
            *channel = 4;
            break;
        case TextureFormat_R8G8:
            *nativeFormat = GL_RG;
            *nativeType = GL_UNSIGNED_BYTE;
            *channel = 2;
            break;
        default:
            GFX_LOG_ERROR("Unsupported format!!");
            *channel = 0;
            break;
        }
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
        case GL_SAMPLER_1D:
            return ProgramDataType_Sampler1D;
        case GL_SAMPLER_2D:
            return ProgramDataType_Sampler2D;
        case GL_SAMPLER_3D:
            return ProgramDataType_Sampler3D;
        case GL_SAMPLER_CUBE:
            return ProgramDataType_SamplerCube;
        case GL_SAMPLER_1D_SHADOW:
            return ProgramDataType_Sampler1DShadow;
        case GL_SAMPLER_2D_SHADOW:
            return ProgramDataType_Sampler2DShadow;
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
            GFX_LOG_ERROR_FMT("GL error detected: %s", error); \
        }                                                     \
    }
#endif
}
