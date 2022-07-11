#include "RenderManager.h"
#include "GL/glew.h"
#include "InternalFunctions.h"

namespace Graphics
{
    RenderManager* RenderManager::mInstance = nullptr;

    RenderManager *RenderManager::Instance()
    {
        if (mInstance != nullptr)
            return mInstance;

        mInstance = new RenderManager();
        return mInstance;
    }

    Buffer::SP RenderManager::AllocBuffer(BufferType type)
    {
        GLuint bufferHandle;
        glGenBuffers(1, &bufferHandle);

        return std::make_shared<Buffer>(bufferHandle, type);
    }

    void RenderManager::ReleaseBuffer(Buffer::SP buffer)
    {
        if (buffer == nullptr)
            return;

        auto handle = buffer->GetBufferHandle();
        glDeleteBuffers(1, &handle);
    }

    ShaderProgram::SP RenderManager::AllocShaderProgram()
    {
        GLuint programHandle = glCreateProgram();
        return std::make_shared<ShaderProgram>(programHandle);
    }

    void RenderManager::ReleaseShaderProgram(ShaderProgram::SP shaderProgram)
    {
        if (shaderProgram == nullptr)
            return;
        glDeleteProgram(shaderProgram->GetProgramHandle());
    }

    void RenderManager::BindBufferRange(Buffer::SP buffer, uint32_t bindPoint, uint32_t offset, uint32_t size)
    {
        glBindBufferRange(GetNativeBufferType(buffer->GetBufferType()), bindPoint, buffer->GetBufferHandle(), offset, size);
        glCheckError();
    }

    void RenderManager::BindBufferBase(Buffer::SP buffer, uint32_t bindPoint)
    {
        glBindBufferBase(GetNativeBufferType(buffer->GetBufferType()), bindPoint, buffer->GetBufferHandle());
        glCheckError();
    }

    void RenderManager::DrawArrays(DrawType type, uint32_t first, uint32_t count)
    {
        glDrawArrays(GetNativeDrawType(type), first, count);
        glCheckError();
    }

    void RenderManager::DrawElements(DrawType type, uint32_t count)
    {
        glDrawElements(GetNativeDrawType(type), count, GL_UNSIGNED_INT, 0);
        glCheckError();
    }

    void RenderManager::Clear(ClearFlag flag)
    {
        GLbitfield bitFlag = 0;
        if (flag & ClearFlag_Color)
            bitFlag |= GL_COLOR_BUFFER_BIT;

        if (flag & ClearFlag_Depth)
            bitFlag |= GL_DEPTH_BUFFER_BIT;

        if (flag & ClearFlag_Stencil)
            bitFlag |= GL_STENCIL_BUFFER_BIT;
        glClear(bitFlag);
        glCheckError();
    }

    typedef void (*EnableFunc)(GLenum);
    static EnableFunc enableFuncs[2] = {glDisable, glEnable};

    void RenderManager::SetRenderStates(const RenderStates &states)
    {
        enableFuncs[states.depthTestEnable](GL_DEPTH_TEST);
        glDepthMask(states.depthWriteEnable);
        if (states.depthTestEnable)
        {
            glDepthFunc(GetNativeDepthStencilFunc(states.depthFunc));
        }

        enableFuncs[states.stencilTestEnable](GL_STENCIL_TEST);
        if (states.stencilTestEnable)
        {
            glStencilMask(states.stencilMask);
            glStencilFunc(GetNativeDepthStencilFunc(states.stencilFunc), states.stencilRef, states.stencilRefMask);
            glStencilOp(GetNativeStencilOp(states.stencilFailOp), GetNativeStencilOp(states.depthFailOp), GetNativeStencilOp(states.depthPassOp));
        }

        enableFuncs[states.blendEnable](GL_BLEND);
        if (states.blendEnable)
        {
            glBlendFunc(GetNativeBlendFunc(states.srcBlendFunc), GetNativeBlendFunc(states.destBlendFunc));
            glBlendEquation(GetNativeBlendEquation(states.blendEquation));
        }

        enableFuncs[states.cullingEnable](GL_CULL_FACE);
        glCullFace(GetNativeCullFace(states.cullFace));
        glCheckError();
    }

    RenderManager::RenderManager()
    {
        int alignment;
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
        RS_LOG_OK_FMT("GL Info:\n    UNIFORM_BUFFER_OFFSET_ALIGNMENT: %d", alignment);
    }

    void RenderManager::ClearColor(Eigen::Vector4f c)
    {
        glClearColor(c[0], c[1], c[2], c[3]);
    }

    void RenderManager::EnableWireFrame(bool enabled)
    {
        if (enabled)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}
