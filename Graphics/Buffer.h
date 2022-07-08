/**
 * @file Buffer.h
 * @author wangyudong
 * @brief Buffer object in graphics, including vertex buffer, index buffer, SSBO, uniform buffer and so on.
 * @version 0.1
 * @date 2022-06-07
 */

#pragma once

#include<memory>
#include "Constants.h"

namespace RenderSystem
{
    class Buffer
    {
    public:
        typedef std::shared_ptr<Buffer> SP;
        Buffer(uint32_t handle, BufferType type) : mBufferHandle(handle), mBufferType(type), mData(nullptr) {}
        ~Buffer() { free(mData); }

        inline BufferType GetBufferType() const { return mBufferType; }
        inline uint32_t GetBufferHandle() const { return mBufferHandle; }

        void BufferData(void *data, size_t size, BufferUsage usage);
        void BufferSubData(void *data, size_t offset, size_t size);

        void Bind() const;
        void BindRange() const;

    private:

        void CopyData(size_t size, void* data);

        uint32_t mBufferHandle = INVALID_ID;
        BufferType mBufferType;

        void* mData;
        size_t mDataSize;
    };
}
