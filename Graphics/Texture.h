/**
 * @file Texture.h
 * @author wangyudong
 * @brief Texture object in graphics, 2d, 3d texture and cube map. TODO: should they be one kind of buffer object?
 * @version 0.1
 * @date 2022-06-07
 */

#pragma once

#include <memory>
#include "Constants.h"

namespace Graphics
{
    class Texture
    {
    public:
        typedef std::shared_ptr<Texture> SP;
        typedef std::shared_ptr<const Texture> CSP;

        Texture(uint32_t handle, TextureType type, TextureFormat format, bool generateMipmap):
            mHandle(handle), mType(type), mFormat(format), mGenerateMipmap(generateMipmap){}
        ~Texture();

        void SetWrapMode(TextureWrapMode S, TextureWrapMode T);
        void SetFilter(TextureFilter min, TextureFilter mag);

        bool TexData(int width, int height, int nchannel, void *data, int level);
        void LoadFromFile(const char *filePath);
        void BindTexture();

        inline uint32_t GetHandle() const { return mHandle; }
        inline void Reset() { mHandle = 0; }

        static Texture::CSP GetWhiteTexture();
        static Texture::CSP GetBlackTexture();
        static Texture::CSP GetMagentaTexture();

    private:
        uint32_t mHandle;
        TextureType mType;
        TextureFormat mFormat;
        void *mData = nullptr;
        bool mGenerateMipmap;

        static Texture::SP mWhiteTexture;
        static Texture::SP mBlackTexture;
        static Texture::SP mMagentaTexture;
    };
}