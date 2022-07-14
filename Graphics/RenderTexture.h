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
        RenderTexture(TextureFormat format);
        void SetUseDepthBuffer(bool useTexture, TextureFormat format);
        void SetUseStencilBuffer();

        // index is a MRT index
        Texture::SP GetColorTexture(int index);
        void SetColorTextureCount(int count);

        void GetDepthTexture();
        void MakeCurrent();

    private:
        Texture::SP mColorTextures[8];
        int mColorTexCount;
    };
}