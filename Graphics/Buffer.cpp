#include "Buffer.h"
#include "GL/glew.h"
#include "stdlib.h"
#include "memory.h"
#include "InternalFunctions.h"

namespace Graphics
{
    void Buffer::BufferData(void *data, size_t size, BufferUsage usage)
    {
        auto nativeType = GetNativeBufferType(mBufferType);
        glBindBuffer(nativeType, mBufferHandle);
        glBufferData(nativeType, size, data, GetNativeBufferUsage(usage));
        glBindBuffer(nativeType, 0);
        CopyData(size, data);
    }

    void Buffer::BufferSubData(void *data, size_t offset, size_t size)
    {
        auto nativeType = GetNativeBufferType(mBufferType);
        glBindBuffer(nativeType, mBufferHandle);
        glBufferSubData(nativeType, offset, size, data);
        glBindBuffer(nativeType, 0);
    }

    void Buffer::CopyData(size_t size, void *data)
    {
        free(mData);
        mData = malloc(size);
        memcpy(mData, data, size);
        mDataSize = size;
    }

    void Buffer::Bind() const
    {
        glBindBuffer(GetNativeBufferType(mBufferType), mBufferHandle);
    }
}
