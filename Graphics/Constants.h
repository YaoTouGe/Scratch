#pragma once

#include <memory>

namespace Graphics
{
    /***********************************************
     * enums
     ***********************************************/
    enum BufferUsage
    {
        BufferUsage_StaticDraw = 0,
        BufferUsage_DynamicDraw,
        BufferUsage_StreamDraw,
        BufferUsage_Max
    };

    enum BufferType
    {
        BufferType_VertexBuffer = 0,
        BufferType_IndexBuffer,
        BufferType_UniformBuffer,
        BufferType_ShaderStorageBuffer,
        BufferType_AtomicCounter,
        BufferType_Max
    };

    // Only support common draw types
    enum DrawType
    {
        DrawType_Triangles = 0,
        DrawType_TriangleStrip,
        DrawType_Lines,
        DrawType_LineStrip,
        DrawType_Max
    };

    enum LayoutName
    {
        LayoutName_None = 0,
        LayoutName_Position = 1,
        LayoutName_Normal = 1 << 1,
        LayoutName_UV0 = 1 << 2,
        LayoutName_UV1 = 1 << 3,
        LayoutName_UV2 = 1 << 4,
        LayoutName_Color0 = 1 << 5,
        LayoutName_Color1 = 1 << 6,
        LayoutName_Color2 = 1 << 7,
        LayoutName_Tangent = 1 << 8,
        LayoutName_Bitangent = 1 << 9,
        LayoutName_Max = LayoutName_Bitangent + 1
    };

    enum ShaderFlag
    {
        ShaderFlag_None = 0,
        ShaderFlag_Vertex = 1,
        ShaderFlag_Geometry = 1 << 1,
        ShaderFlag_TesselationControl = 1 << 2,
        ShaderFlag_TesselationEval = 1 << 3,
        ShaderFlag_Fragment = 1 << 4,
        ShaderFlag_Max = ShaderFlag_Fragment + 1
    };

    enum ProgramDataType
    {
        ProgramDataType_None = 0,
        ProgramDataType_Float,
        ProgramDataType_Int,
        ProgramDataType_Vec2,
        ProgramDataType_Vec3,
        ProgramDataType_Vec4,
        ProgramDataType_Mat4,
        ProgramDataType_SamplerStart,
        ProgramDataType_Sampler1D = ProgramDataType_SamplerStart,
        ProgramDataType_Sampler2D,
        ProgramDataType_Sampler3D,
        ProgramDataType_SamplerCube,
        ProgramDataType_Sampler1DShadow,
        ProgramDataType_Sampler2DShadow,
        ProgramDataType_SamplerEnd = ProgramDataType_Sampler2DShadow
    };

    enum DepthStencilFunc
    {
        DepthStencilFunc_Always = 0,
        DepthStencilFunc_Never,
        DepthStencilFunc_Less,
        DepthStencilFunc_Greater,
        DepthStencilFunc_Equal,
        DepthStencilFunc_NotEqual,
        DepthStencilFunc_LEqual,
        DepthStencilFunc_GEqual,
        DepthStencilFunc_Max
    };

    enum StencilOp
    {
        StencilOp_Keep = 0,
        StencilOp_Zero,
        StencilOp_Replace,
        StencilOp_Increase,
        StencilOp_IncreaseWrap,
        StencilOp_Decrease,
        StencilOp_DecreaseWrap,
        StencilOp_Invert,
        StencilOp_Max
    };

    enum BlendFunc
    {
        BlendFunc_Zero = 0,
        BlendFunc_One,
        BlendFunc_SrcColor,
        BlendFunc_OneMinusSrcColor,
        BlendFunc_DestColor,
        BlendFunc_OneMinusDestColor,
        BlendFunc_SrcAlpha,
        BlendFunc_OneMinusSrcAlpha,
        BlendFunc_DestAlpha,
        BlendFunc_OneMinusDestAlpha,
        BlendFunc_ConstantColor,
        BlendFunc_OneMinusConstantColor,
        BlendFunc_ConstantAlpha,
        BlendFunc_OneMinusConstantAlpha,
        BlendFunc_Max
    };

    enum BlendEquation
    {
        BlendEquation_Add = 0,
        BlendEquation_Subtract,
        BlendEquation_ReverseSubTract,
        BlendEquation_Max
    };

    enum CullFace
    {
        CullFace_Front = 0,
        CullFace_Back,
        CullFace_FrontAndBack,
        CullFace_Max
    };

    enum ClearFlag
    {
        ClearFlag_Color = 1,
        ClearFlag_Depth = 1 << 1,
        ClearFlag_Stencil = 1 << 2,
        ClearFlag_All = ClearFlag_Color | ClearFlag_Depth | ClearFlag_Stencil
    };

    enum TextureType
    {
        TextureType_2D = 0,
        TextureType_3D,
        TextureType_Cube
    };

    enum TextureFormat
    {
        TextureFormat_R8G8B8A8 = 0,
        TextureFormat_R8G8B8,
        TextureFormat_R8G8,
    };

    enum TextureFilter
    {
        TextureFilter_Linear = 0,
        TextureFilter_Nearest,
        TextureFilter_LinearMipmapLinear,
        TextureFilter_LinearMipmapNearest,
        TextureFilter_NearestMimapNearest,
        TextureFilter_NearestMipmapLinear,
        TextureFilter_Max
    };

    enum TextureWrapMode
    {
        TextureWrapMode_ClampToEdge = 0,
        TextureWrapMode_ClampToBorder,
        TextureWrapMode_Repeat,
        TextureWrapMode_MirroredRepeat,
        TextureWrapMode_Max
    };

    /***********************************************
     * shader constants
     ***********************************************/

    #define PriorityOpaque 1000
    #define PriorityTransparent 2000
    #define PriorityPostEffects 3000

    #define GlobalUBOBindPoint 0
    #define PerMaterialUBOBindPoint 1
    #define PerObjectUBOBindPoint 2

    #define GlobalUBOName "Globals"
    #define PerMaterialUBOName "PerMaterial"
    #define PerObjectUBOName "PerObject"
    #define INVALID_ID 0xffffffff

    /***********************************************
     * helper funcs
     ***********************************************/
    #define PRINT_HEADER  "\033[95m"
    #define PRINT_OKBLUE  "\033[94m"
    #define PRINT_OKCYAN  "\033[96m"
    #define PRINT_OKGREEN  "\033[92m"
    #define PRINT_WARNING  "\033[93m"
    #define PRINT_FAIL  "\033[91m"
    #define PRINT_ENDC  "\033[0m"
    #define PRINT_BOLD  "\033[1m"
    #define PRINT_UNDERLINE  "\033[4m"

#define GFX_LOG_OK(msg)                                          \
    {                                                           \
        printf("%s%s%s\n", PRINT_OKGREEN, msg, PRINT_ENDC);     \
    }

#define GFX_LOG_ERROR(msg)                                                            \
    {                                                                                \
        printf("%s%s(%d): %s%s\n", PRINT_FAIL, __FILE__, __LINE__, msg, PRINT_ENDC); \
    }

#define GFX_LOG_OK_FMT(msg_fmt, ...)         \
    {                                       \
        printf(PRINT_OKGREEN);              \
        printf(msg_fmt, ##__VA_ARGS__);     \
        printf("%s\n", PRINT_ENDC);         \
    }

#define GFX_LOG_ERROR_FMT(msg_fmt, ...)                       \
    {                                                        \
        printf("%s%s(%d): ", PRINT_FAIL, __FILE__, __LINE__);\
        printf(msg_fmt, ##__VA_ARGS__);                      \
        printf("%s\n", PRINT_ENDC);                          \
    }

    /***********************************************
     * math constants
     ***********************************************/

#define PI 3.1415926535897932
#define PI_F 3.1415926535897932f

#define PI_2 1.57079632679489661923
#define PI_2_F 1.57079632679489661923f

#define DegreesToRadians PI / 180
#define RadiansToDegrees 180 / PI

#define DegreesToRadians_F PI_F / 180
#define RadiansToDegrees_F 180 / PI_F
}
