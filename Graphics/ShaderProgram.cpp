#include "ShaderProgram.h"
#include <iostream>
#include <algorithm>

#include "GL/glew.h"
#include "Constants.h"
#include "InternalFunctions.h"

namespace Graphics
{
    void ShaderProgram::SetVertexShaderSource(const char *src)
    {
        mStageHandles.vertexHandle = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(mStageHandles.vertexHandle, 1, &src, nullptr);
        mStageFlag |= ShaderFlag_Vertex;
    }

    void ShaderProgram::SetFragmentShaderSource(const char *src)
    {
        mStageHandles.fragmentHandle = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(mStageHandles.fragmentHandle, 1, &src, nullptr);
        mStageFlag |= ShaderFlag_Fragment;
    }

    bool ShaderProgram::BuildProgram()
    {
        if (mHaveBuilt)
            return true;

        for (int i = 0, flag = 1; flag < ShaderFlag_Max; i++, flag <<= 1)
        {
            if (mStageFlag & flag)
            {
                glCompileShader(mStageHandles.handles[i]);
                glAttachShader(mProgramHandle, mStageHandles.handles[i]);
            }
        }
        glLinkProgram(mProgramHandle);

        GLint success;
        glGetProgramiv(mProgramHandle, GL_LINK_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetProgramInfoLog(mProgramHandle, 512, NULL, infoLog);
            GFX_LOG_ERROR(infoLog);
            return false;
        }

        for (int i = 0, flag = 1; flag < ShaderFlag_Max; i++, flag <<= 1)
        {
            if (mStageFlag & flag)
            {
                glDetachShader(mProgramHandle, mStageHandles.handles[i]);
            }
        }

        mGlobalUniformBlockIdx = glGetUniformBlockIndex(mProgramHandle, GlobalUBOName);
        if (mGlobalUniformBlockIdx == GL_INVALID_INDEX)
        {
            GFX_LOG_ERROR("Can't find gloabl uniform block!");
        }

        mPerMaterialUniformBlockIdx = glGetUniformBlockIndex(mProgramHandle, PerMaterialUBOName);
        if (mPerMaterialUniformBlockIdx == GL_INVALID_INDEX)
        {
            GFX_LOG_ERROR("Can't find per material uniform block!");
        }

        mPerObjectUniformBlockIdx = glGetUniformBlockIndex(mProgramHandle, PerObjectUBOName);
        if (mPerObjectUniformBlockIdx == GL_INVALID_INDEX)
        {
            GFX_LOG_ERROR("Can't find per object uniform block!");
        }

        // Set uniform block index binding, UBOs will be bound to the same point when drawing.
        glUniformBlockBinding(mProgramHandle, mGlobalUniformBlockIdx, GlobalUBOBindPoint);
        glUniformBlockBinding(mProgramHandle, mPerMaterialUniformBlockIdx, PerMaterialUBOBindPoint);
        glUniformBlockBinding(mProgramHandle, mPerObjectUniformBlockIdx, PerObjectUBOBindPoint);
        
        mHaveBuilt = true;

        auto property = GetPropertyLayout();
        // property->PrintLayoutInfos();
        // set sampler uniforms after built, sort by texture name
        mSamplerInfos.clear();
        for (auto uniform: property->uniformInfos)
        {
            if (uniform.type >= ProgramDataType_SamplerStart
                && uniform.type <= ProgramDataType_SamplerEnd)
            {
                mSamplerInfos.push_back(uniform);
            }
        }

        std::sort(mSamplerInfos.begin(), mSamplerInfos.end(), 
        [](ShaderProgramPropertyLayout::UniformInfo info1, ShaderProgramPropertyLayout::UniformInfo info2) -> int
        {
            return info1.name.compare(info2.name);
        });

        glUseProgram(mProgramHandle);
        for(int i = 0; i < mSamplerInfos.size(); ++i)
        {
            glUniform1i(mSamplerInfos[i].location, i);
        }
        glUseProgram(0);

        return true;
    }

    void ShaderProgram::UseProgram()
    {
        BuildProgram();
        glUseProgram(mProgramHandle);
    }

    ShaderProgramPropertyLayout::SP ShaderProgram::GetPropertyLayout()
    {
        BuildProgram();
        if (mPropertyLayout == nullptr)
        {
            mPropertyLayout = ShaderProgramPropertyLayout::BuildFromProgram(*this);
        }

        return mPropertyLayout;
    }

    ShaderProgramPropertyLayout::SP ShaderProgramPropertyLayout::BuildFromProgram(const ShaderProgram &program)
    {
        auto ret = std::make_shared<ShaderProgramPropertyLayout>();

        auto handle = program.GetProgramHandle();
        int outBlockUniformCount = 0;

        int count;
        glGetProgramInterfaceiv(handle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &count);
        const GLenum uniformProps[] = { GL_BLOCK_INDEX, GL_LOCATION, GL_NAME_LENGTH, GL_TYPE};
        std::vector<char> nameBuffer;

        for(int idx = 0; idx < count; ++idx)
        {
            GLint propValues[4];
            glGetProgramResourceiv(handle, GL_UNIFORM, idx, 4, uniformProps, 4, nullptr, propValues);

            // Skip in block uniforms
            if (propValues[0] != -1)
                continue;

            UniformInfo info;
            nameBuffer.resize(propValues[2]);
            glGetProgramResourceName(handle, GL_UNIFORM, idx, propValues[2], nullptr, &nameBuffer[0]);
            info.location = propValues[1];
            info.type = GLType2ProgramDataType(propValues[3]);
            info.name.assign(nameBuffer.begin(), nameBuffer.end() - 1);
            ret->uniformInfos.push_back(info);
        }

        glGetProgramInterfaceiv(handle, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &count);
        const GLenum blockProps[] = {GL_NUM_ACTIVE_VARIABLES, GL_NAME_LENGTH, GL_BUFFER_DATA_SIZE};
        const GLenum blockUniformIdxProps[] = {GL_ACTIVE_VARIABLES};
        const GLenum inBlockUniformProps[] = {GL_NAME_LENGTH, GL_OFFSET};

        for (int idx = 0; idx < count; ++idx)
        {
            GLint values[3];
            glGetProgramResourceiv(handle, GL_UNIFORM_BLOCK, idx, 3, blockProps, 3, nullptr, &values[0]);

            // no active uniforms in block
            if (!values[0])
                continue;
            UniformBlockInfo blockInfo;
            nameBuffer.resize(values[1]);
            glGetProgramResourceName(handle, GL_UNIFORM_BLOCK, idx, values[1], nullptr, &nameBuffer[0]);
            blockInfo.blockSize = values[2];
            blockInfo.blockName.assign(nameBuffer.begin(), nameBuffer.end() - 1);
            blockInfo.uniformNames.resize(values[0]);
            blockInfo.uniformOffset.resize(values[0]);
            std::vector<int> uniformIdx(values[0]);

            glGetProgramResourceiv(handle, GL_UNIFORM_BLOCK, idx, 1, blockUniformIdxProps, values[0], nullptr, &uniformIdx[0]);

            for (int uniIdx = 0; uniIdx < values[0]; ++uniIdx)
            {
                int uniformPropValues[2];
                glGetProgramResourceiv(handle, GL_UNIFORM, uniformIdx[uniIdx], 2, inBlockUniformProps, 2, nullptr, &uniformPropValues[0]);
                nameBuffer.resize(uniformPropValues[0]);
                glGetProgramResourceName(handle, GL_UNIFORM, uniformIdx[uniIdx], uniformPropValues[0], nullptr, &nameBuffer[0]);
                blockInfo.uniformOffset[uniIdx] = uniformPropValues[1];
                blockInfo.uniformNames[uniIdx].assign(nameBuffer.begin(), nameBuffer.end() - 1);
            }

            ret->uniformBlockInfos.push_back(blockInfo);
        }

        return ret;
    }

    void ShaderProgramPropertyLayout::PrintLayoutInfos()
    {
        for (size_t i = 0; i < uniformInfos.size(); ++i)
        {
            GFX_LOG_OK_FMT("%s: location %d, type %d", uniformInfos[i].name.data(), uniformInfos[i].location, uniformInfos[i].type);
        }

        for (size_t i = 0; i < uniformBlockInfos.size(); ++i)
        {
            auto& blockInfo = uniformBlockInfos[i];
            GFX_LOG_OK_FMT("%s: size %d", blockInfo.blockName.data(), (int)blockInfo.blockSize);
            for (size_t j = 0; j < blockInfo.uniformNames.size(); ++j)
            {
                GFX_LOG_OK_FMT("    %s: offset %d", blockInfo.uniformNames[j].data(), (int)blockInfo.uniformOffset[j]);
            }
        }
    }
}
