#include "Texture.h"
#include "string.h"
#include "GL/glew.h"
#include "InternalFunctions.h"
#include "stb/stb_image.h"
#include "RenderManager.h"

namespace Graphics
{
    // currently only 2d, others supported lately
    void Texture::SetWrapMode(TextureWrapMode S, TextureWrapMode T)
    {
        glBindTexture(GL_TEXTURE_2D, mHandle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetNativeWrapMode(S));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetNativeWrapMode(T));
    }

    void Texture::SetFilter(TextureFilter min, TextureFilter mag)
    {
        glBindTexture(GL_TEXTURE_2D, mHandle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetNativeFilter(min));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetNativeFilter(mag));
    }

    void Texture::BindTexture()
    {
        glBindTexture(GL_TEXTURE_2D, mHandle);
    }

    bool Texture::TexData(int width, int height, int nchannel, void *data, int level)
    {
        uint32_t nativeType;
        uint32_t nativeFormat;
        int formatChannel;
        GetNativeTypeAndFormat(mFormat, &nativeType, &nativeFormat, &formatChannel);

        if (data != nullptr)
        {
            if (formatChannel != nchannel)
            {
                GFX_LOG_ERROR("texture data channel not match!");
                return false;
            }

            auto dataSize = nchannel * width * height;
            mData = malloc(dataSize);
            memcpy(mData, data, dataSize);
        }

        glBindTexture(GL_TEXTURE_2D, mHandle);
        glTexImage2D(GL_TEXTURE_2D, 0, nativeFormat, width, height, 0, nativeFormat, nativeType, data);

        if (mGenerateMipmap)
            glGenerateMipmap(GL_TEXTURE_2D);

        return true;
    }

    void Texture::LoadFromFile(const char *filePath)
    {
        int w, h, nchannel;
        unsigned char *data = stbi_load(filePath, &w, &h, &nchannel, 0);

        if (data == nullptr)
            GFX_LOG_ERROR_FMT("load image %s failed!", filePath);

        if (!TexData(w, h, nchannel, data, 0))
        {
            GFX_LOG_ERROR_FMT("set image data failed! %s", filePath);
        }
    }

    Texture::SP Texture::mWhiteTexture = nullptr;
    Texture::SP Texture::mBlackTexture = nullptr;
    Texture::SP Texture::mMagentaTexture = nullptr;

    Texture::CSP Texture::GetWhiteTexture()
    {
        if (mWhiteTexture == nullptr)
        {
            mWhiteTexture = RenderManager::Instance()->AllocTexture(TextureType_2D, TextureFormat_R8G8B8A8, false);
            mWhiteTexture->SetFilter(TextureFilter_Linear, TextureFilter_Linear);
            mWhiteTexture->SetWrapMode(TextureWrapMode_Repeat, TextureWrapMode_Repeat);
            uint8_t pixels[16] = {
                255, 255, 255, 255,
                255, 255, 255, 255,
                255, 255, 255, 255,
                255, 255, 255, 255};
            mWhiteTexture->TexData(2, 2, 4, pixels, 0);
        }

        return mWhiteTexture;
    }

    Texture::CSP Texture::GetBlackTexture()
    {
        if (mBlackTexture == nullptr)
        {
            mBlackTexture = RenderManager::Instance()->AllocTexture(TextureType_2D, TextureFormat_R8G8B8A8, false);
            mBlackTexture->SetFilter(TextureFilter_Linear, TextureFilter_Linear);
            mBlackTexture->SetWrapMode(TextureWrapMode_Repeat, TextureWrapMode_Repeat);
            uint8_t pixels[16] = {
                0,
                0,
                0,
                255,
                0,
                0,
                0,
                255,
                0,
                0,
                0,
                255,
                0,
                0,
                0,
                255,
            };
            mBlackTexture->TexData(2, 2, 4, pixels, 0);
        }

        return mBlackTexture;
    }

    Texture::CSP Texture::GetMagentaTexture()
    {
        if (mMagentaTexture == nullptr)
        {
            mMagentaTexture = RenderManager::Instance()->AllocTexture(TextureType_2D, TextureFormat_R8G8B8A8, false);
            mMagentaTexture->SetFilter(TextureFilter_Linear, TextureFilter_Linear);
            mMagentaTexture->SetWrapMode(TextureWrapMode_Repeat, TextureWrapMode_Repeat);
            uint8_t pixels[16] = {
                255,
                0,
                255,
                255,
                255,
                0,
                255,
                255,
                255,
                0,
                255,
                255,
                255,
                0,
                255,
                255,
            };
            mMagentaTexture->TexData(2, 2, 4, pixels, 0);
        }

        return mMagentaTexture;
    }

    Texture::~Texture()
    {
        free(mData);
        RenderManager::Instance()->ReleaseTexture(this);
    }
}