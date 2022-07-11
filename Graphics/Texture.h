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
        Texture(uint32_t handle, TextureType type, TextureFormat format, bool generateMipmap):
            mHandle(handle), mType(type), mFormat(format), mGenerateMipmap(generateMipmap){}
        ~Texture() { free(mData); }

        void SetWrapMode(TextureWrapMode S, TextureWrapMode T);
        void SetFilter(TextureFilter min, TextureFilter mag);

        void TexData(int width, int height, int nchannel, void *data, int level);
        void LoadFromFile(const char *filePath);
        void BindTexture();

        uint32_t GetHandle() { return mHandle; }

    private:
        uint32_t mHandle;
        TextureType mType;
        TextureFormat mFormat;
        void *mData = nullptr;
        bool mGenerateMipmap;
    };
}