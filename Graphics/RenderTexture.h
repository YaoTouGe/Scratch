/**
 * @file RenderTexutre.h
 * @author wangyudong
 * @brief Used as render target to do off-screen rendering and so on, just like FBO.
 * @version 0.1
 * @date 2022-06-07
 */

#pragma once

#include <memory>
#include "Texture.h"

namespace Graphics
{
    // Actually a frame buffer object
    class RenderTexture
    {
    public:
        typedef std::shared_ptr<RenderTexture>  SP;
        RenderTexture(int fboHandle, TextureFormat colorFormat, int colorBufferCount, bool useDepthStencil, int width, int height);
        ~RenderTexture();

        // index is a MRT index
        Texture::SP GetColorTexture(int index);
        void MakeCurrent();

    private:
        Texture::SP mColorTextures[8];
        int mColorTexCount;
        int mFBOHandle;
        int mDepthStencilHandle;
        bool mUseDepthStencil;
    };
}