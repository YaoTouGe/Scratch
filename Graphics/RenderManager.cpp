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

    Texture::SP RenderManager::AllocTexture(TextureType type, TextureFormat format, bool generateMipmap)
    {
        GLuint texHandle;
        glGenTextures(1, &texHandle);

        return std::make_shared<Texture>(texHandle, type, format, generateMipmap);
    }

    ShaderProgram::SP RenderManager::AllocShaderProgram()
    {
        GLuint programHandle = glCreateProgram();
        return std::make_shared<ShaderProgram>(programHandle);
    }

    RenderTexture::SP AllocRenderTexture(TextureFormat format, int rtCount, bool useDepthStencil, int width, int height)
    {
        GLuint fboHandle;
        glGenFramebuffers(1, &fboHandle);

        return std::make_shared<RenderTexture>(fboHandle, format, rtCount, useDepthStencil, width, height);
    }

    void RenderManager::ReleaseTexture(Texture *tex)
    {
        if (tex == nullptr)
            return;
        auto handle = tex->GetHandle();
        glDeleteTextures(1, &handle);
        tex->Reset();
    }

    void RenderManager::ReleaseBuffer(Buffer *buffer)
    {
        if (buffer == nullptr)
            return;

        auto handle = buffer->GetBufferHandle();
        glDeleteBuffers(1, &handle);
        buffer->Reset();
    }

    void RenderManager::ReleaseShaderProgram(ShaderProgram *shaderProgram)
    {
        if (shaderProgram == nullptr)
            return;
        glDeleteProgram(shaderProgram->GetProgramHandle());
        shaderProgram->Reset();
    }

    void RenderManager::ReleaseRenderTexture(RenderTexture *rt)
    {
        if (rt == nullptr)
            return;

        GLuint handle = (GLuint)rt->GetHandle();
        glDeleteFramebuffers(1, &handle);
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

#ifdef _MSC_VER
    typedef void (__stdcall *EnableFunc)(GLenum);
#else
    typedef void(*EnableFunc)(GLenum);
#endif
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
        GraphicsInfo info;
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &info.uniformBufferOffsetAlignment);
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &info.maxTextureImageUnits);
        glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &info.maxVertexTextureImageUnits);

        info.printInfo();
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
