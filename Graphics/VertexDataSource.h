/**
 * @file VertexDataSource.h
 * @author wangyudong
 * @brief VertexDataSource provides vertex data stream for render node.
 * @version 0.1
 * @date 2022-06-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <memory>

namespace RenderSystem
{
    class VertexDataSource
    {    
    public:

        typedef std::shared_ptr<VertexDataSource> SP;
        virtual void Prepare() = 0;
        virtual void Bind() = 0;

        inline size_t VertexCount() { return mVertexCount; }
        inline size_t IndexCount() { return mIndexCount; }
        inline bool HasIndex() { return mHasIndex; }

    protected:
        size_t mVertexCount = 0;
        size_t mIndexCount = 0;
        bool mHasIndex = false;
    };
}