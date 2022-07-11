#include "Texture.h"
#include "string.h"
#include "GL/glew.h"
#include "InternalFunctions.h"
#include "stb/stb_image.h"

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

    void Texture::TexData(int width, int height, int nchannel, void *data, int level)
    {
        uint32_t nativeType;
        uint32_t nativeFormat;

        int formatChannel;
        GetNativeTypeAndFormat(mFormat, &nativeType, &nativeFormat, &formatChannel);
        if (formatChannel != nchannel)
        {
            GFX_LOG_ERROR("texture data channel not match!");
            return;
        }

        auto dataSize = nchannel *width *height;
        mData = malloc(dataSize);
        memcpy(mData, data, dataSize);

        glBindTexture(GL_TEXTURE_2D, mHandle);
        glTexImage2D(GL_TEXTURE_2D, 0, nativeFormat, width, height, 0, nativeFormat, nativeType, data);

        if (mGenerateMipmap)
            glGenerateMipmap(GL_TEXTURE_2D);
    }

    void Texture::LoadFromFile(const char *filePath)
    {
        int w, h, nchannel;
        unsigned char *data = stbi_load(filePath, &w, &h, &nchannel, 0);

        if (data == nullptr)
            GFX_LOG_ERROR_FMT("load image %s failed!", filePath);

        TexData(w, h, nchannel, data, 0);
    }
}