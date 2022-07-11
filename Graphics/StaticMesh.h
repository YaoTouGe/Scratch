/**
 * @file StaticMesh.h
 * @author wangyudong
 * @brief A collection of buffers(vertex data) used to render.
 * @version 0.1
 * @date 2022-06-07
 */

#pragma once

#include <memory>
#include <Eigen/Core>
#include <vector>
#include "Buffer.h"
#include "Constants.h"
#include "VertexDataSource.h"

namespace Graphics
{
    /**
     * @brief Represent a static mesh with fixed vertex attribute layout:
     * location 0 position, 1 normal, 2 uv0, 3 uv1, 4 uv2, 5 color0, 6 color1, 7 color2, (TODO: 8 Tangent, 9 BiTangent)
     */
    class StaticMesh: public VertexDataSource
    {
    public:
        typedef std::shared_ptr<StaticMesh> SP;

        StaticMesh() {}
        ~StaticMesh();

        void SetPositions(const std::vector<Eigen::Vector3f>& positions)
        {
            mPositions = positions;
            mLayoutFlag |= LayoutName_Position;
            mDirty = true;
        }

        void SetPositions(std::vector<Eigen::Vector3f> &&positions)
        {
            mPositions.swap(positions);
            mLayoutFlag |= LayoutName_Position;
            mDirty = true;
        }

        void SetNormals(const std::vector<Eigen::Vector3f> &normals)
        {
            mNormals = normals;
            mLayoutFlag |= LayoutName_Normal;
            mDirty = true;
        }

        void SetNormals(std::vector<Eigen::Vector3f> &&normals)
        {
            mNormals.swap(normals);
            mLayoutFlag |= LayoutName_Normal;
            mDirty = true;
        }

        void SetUvs(const std::vector<Eigen::Vector3f> &uv, int uvChannel)
        {
            if (uvChannel > 2 || uvChannel < 0)
                return;
            mUvs[uvChannel] = uv;
            mLayoutFlag |= (LayoutName_UV0 << uvChannel);
            mDirty = true;
        }

        void SetUvs(std::vector<Eigen::Vector3f>&& uv, int uvChannel)
        {
            if (uvChannel > 2 || uvChannel < 0)
                return;
            mUvs[uvChannel].swap(uv);
            mLayoutFlag |= (LayoutName_UV0 << uvChannel);
            mDirty = true;
        }

        void SetVeretxColors(const std::vector<Eigen::Vector3f>& color, int colorChannel)
        {
            if (colorChannel > 2 || colorChannel < 0)
                return;
            mColors[colorChannel] = color;
            mLayoutFlag |= (LayoutName_Color0 << colorChannel);
            mDirty = true;
        }

        void SetVeretxColors(std::vector<Eigen::Vector3f>&& color, int colorChannel)
        {
            if (colorChannel > 2 || colorChannel < 0)
                return;
            mColors[colorChannel].swap(color);
            mLayoutFlag |= (LayoutName_Color0 << colorChannel);
            mDirty = true;
        }

        void SetIndices(const std::vector<uint32_t>& indices)
        {
            mIndices = indices;
            mHasIndex = true;
            mDirty = true;
        }

        void SetIndices(std::vector<uint32_t>&& indices)
        {
            mIndices.swap(indices);
            mHasIndex = true;
            mDirty = true;
        }

        void Prepare() override;
        void Bind() override;

    private:

        uint32_t mLayoutFlag = LayoutName_None;

        std::vector<Eigen::Vector3f> mPositions;
        std::vector<Eigen::Vector3f> mNormals;
        std::vector<Eigen::Vector3f> mUvs[3]; // 3d uv, may use only 2d in most case
        std::vector<Eigen::Vector3f> mColors[3];

        std::vector<uint32_t> mIndices;

        Buffer::SP mVertexBuffer = nullptr;
        Buffer::SP mIndexBuffer = nullptr;
        uint32_t mVAOHandle = INVALID_ID;

        bool mDirty = true;
        void* mPreparedBuffer = nullptr;
        size_t mPreparedBufferSize = 0;

        const size_t mAttributeStride = sizeof(float) * 3;
    };
}
