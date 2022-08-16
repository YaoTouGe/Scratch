#include "RenderTexture.h"
#include "RenderManager.h"

namespace Graphics
{
    RenderTexture::RenderTexture(int fboHandle, TextureFormat format, int colorBufferCount, bool useDepthStencil, int width, int height)
    {
        auto rm = RenderManager::Instance();
        mColorTexCount = colorBufferCount;
        mUseRenderBuffer = useDepthStencil;

        if (colorBufferCount > 8)
        {
            GFX_LOG_ERROR_FMT("Too many color buffers, %d more than 8", colorBufferCount);
            return;
        }

        for (int i = 0; i < colorBufferCount; ++i)
        {
            mColorTextures[i] = rm->AllocTexture(TextureType_2D, format, false);
            mColorTextures[i]->TexData(width, height, 0, null, 0);
        }

        mFBOHandle = fboHandle;

        glBindFramebuffer(GL_FRAMEBUFFER, mFBOHandle);

        for (int i = 0; i < colorBufferCount; ++i)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, mColorTextures[i]->GetHandle());
        }

        if (useDepthStencil)
        {
            glGenRenderbuffers(1, &mDepthStencilHandle);
            glBindRenderbuffer(GL_RENDERBUFFER, mDepthStencilHandle);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthStencilHandle);
        }
    }

    Texture::SP GetColorTexture(int index)
    {
        if (index >= mColorTexCount)
        {
            GFX_LOG_ERROR_FMT("Index %d more than actual %d", index, colorBufferCount);
            return nullptr;
        }
        return mColorTextures[index];
    }

    Texture::~Texture()
    {
        glDeleteFramebuffers(1, &mFBOHandle);
    }
}