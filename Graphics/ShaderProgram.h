/**
 * @file ShaderProgram.h
 * @author wangyudong
 * @brief ShaderProgram is assembled by a variety of shader stages,
 * including vertex, geometry, tesselation, fragment shaders ( compute and ray tracing shaders later ).
 * @version 0.1
 * @date 2022-06-07
 */

#pragma once

#include <memory>
#include <vector>
#include "Constants.h"

namespace Graphics
{
    class ShaderProgram;
    struct ShaderProgramPropertyLayout
    {
        typedef std::shared_ptr<ShaderProgramPropertyLayout> SP;

        static ShaderProgramPropertyLayout::SP BuildFromProgram(const ShaderProgram &program);

        struct UniformBlockInfo
        {
            std::string blockName;
            size_t blockSize;
            std::vector<std::string> uniformNames;
            std::vector<size_t> uniformOffset;
        };

        struct UniformInfo
        {
            std::string name;
            int location;
            ProgramDataType type;
        };

        void PrintLayoutInfos();

        std::vector<UniformBlockInfo> uniformBlockInfos;
        std::vector<UniformInfo> uniformInfos;
    };

    // frequently used states are packed here
    struct RenderStates
    {
        bool depthTestEnable;           // default true
        bool depthWriteEnable;          // default true
        DepthStencilFunc depthFunc;     // default less

        bool stencilTestEnable;         // default false
        uint32_t stencilMask;           // default 0xFFFFFFFF
        DepthStencilFunc stencilFunc;   // default Always
        int stencilRef;                 // default 1
        uint32_t stencilRefMask;        // default 0xFFFFFFFF

        StencilOp stencilFailOp; // default keep
        StencilOp depthFailOp;   // default keep
        StencilOp depthPassOp;   // default keep

        bool blendEnable;               // default false
        BlendFunc srcBlendFunc;         // default SrcAlpha
        BlendFunc destBlendFunc;        // default OneMinusSrcAlpha
        BlendEquation blendEquation;    // default Add

        bool cullingEnable;             // default true
        CullFace cullFace;              // default Back

        void Reset()
        {
            depthTestEnable = true;
            depthWriteEnable = true;
            depthFunc = DepthStencilFunc_Less;

            stencilTestEnable = false;
            stencilMask = 0xFFFFFFFF;
            stencilFunc = DepthStencilFunc_Always;
            stencilRef = 1;
            stencilRefMask = 0xFFFFFFFF;

            stencilFailOp = StencilOp_Keep;
            depthFailOp = StencilOp_Keep;
            depthPassOp = StencilOp_Keep;

            blendEnable = false;
            srcBlendFunc = BlendFunc_SrcAlpha;
            destBlendFunc = BlendFunc_OneMinusSrcAlpha;
            blendEquation = BlendEquation_Add;

            cullingEnable = true;
            cullFace = CullFace_Back;
        }
    };

    class ShaderProgram
    {
    public:
        typedef std::shared_ptr<ShaderProgram> SP;

        ShaderProgram(uint32_t programHandle) { mProgramHandle = programHandle; }
        ~ShaderProgram() {}

        uint32_t GetProgramHandle() const { return mProgramHandle; }
        void SetVertexShaderSource(const char* src);
        void SetFragmentShaderSource(const char *src);

        ShaderProgramPropertyLayout::SP GetPropertyLayout();

        bool BuildProgram();
        void UseProgram();
        
        void SetStates(const RenderStates &states) { mStates = states;}
        const RenderStates &GetStates() { return mStates; }

        const std::vector<ShaderProgramPropertyLayout::UniformInfo> &GetSamplerInfos() { return mSamplerInfos; }
    private:

        uint32_t mProgramHandle = -1;

        struct ShaderStageHandles
        {
            union
            {
                uint32_t handles[5];
                struct
                {
                    uint32_t vertexHandle;
                    uint32_t geometryHandle;
                    uint32_t tessControlHandle;
                    uint32_t tessEvalHandle;
                    uint32_t fragmentHandle;
                };
            };
        };

        ShaderStageHandles mStageHandles = {INVALID_ID, INVALID_ID, INVALID_ID, INVALID_ID, INVALID_ID};
        uint32_t mStageFlag = ShaderFlag_None;
        bool mHaveBuilt = false;

        uint32_t mGlobalUniformBlockIdx = INVALID_ID;
        uint32_t mPerMaterialUniformBlockIdx = INVALID_ID;
        uint32_t mPerObjectUniformBlockIdx = INVALID_ID;
        ShaderProgramPropertyLayout::SP mPropertyLayout = nullptr;
        std::vector<ShaderProgramPropertyLayout::UniformInfo> mSamplerInfos;
        RenderStates mStates;
    };
}
