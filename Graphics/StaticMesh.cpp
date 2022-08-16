#include "StaticMesh.h"
#include "RenderManager.h"
#include "GL/glew.h"
#include <iostream>
#include "Eigen/LU"

namespace Graphics
{
    void StaticMesh::Prepare()
    {
        if (!mDirty)
            return;

        CalculateTBN();
        // TODO: dirty mark and only upload changed data.
        if (mVertexBuffer == nullptr)
            mVertexBuffer = RenderManager::Instance()->AllocBuffer(BufferType_VertexBuffer);

        // Assemble vertex buffer according to layout flags
        int totalStride = 0;
        for (int i = 0, flag = 1; flag < LayoutName_Max; i++, flag <<= 1)
        {
            if (mLayoutFlag & flag)
            {
                totalStride += mAttributeStride;
            }
        }

        // Populate data
        mPreparedBufferSize = totalStride * mPositions.size();
        free(mPreparedBuffer);
        mPreparedBuffer = malloc(mPreparedBufferSize);

        char* writeStart = (char*)mPreparedBuffer;
        std::vector<Eigen::Vector3f> *attribs[] = {&mPositions, &mNormals, &mUvs[0], &mUvs[1], &mUvs[2], &mColors[0], &mColors[1], &mColors[2], &mTangents, &mBiTangents};

        for (int i = 0, flag = 1; flag < LayoutName_Max; i++, flag <<= 1)
        {
            if (mLayoutFlag & flag)
            {
                auto writePtr = (float *)writeStart;
                for (size_t j = 0; j < attribs[i]->size(); ++j)
                {
                    *writePtr = attribs[i]->at(j)[0];
                    *(writePtr + 1) = attribs[i]->at(j)[1];
                    *(writePtr + 2) = attribs[i]->at(j)[2];
                    writePtr = (float *)((char *)writePtr + totalStride);
                }
                writeStart = writeStart + mAttributeStride;
            }
        }

        mVertexBuffer->BufferData(mPreparedBuffer, mPreparedBufferSize, BufferUsage_StaticDraw);

        if (mHasIndex)
        {
            if (mIndexBuffer == nullptr)
                mIndexBuffer = RenderManager::Instance()->AllocBuffer(BufferType_IndexBuffer);
            mIndexBuffer->BufferData(mIndices.data(), mIndices.size() * sizeof(uint32_t), BufferUsage_StaticDraw);
        }

        // Set VAO
        if (mVAOHandle == INVALID_ID)
            glGenVertexArrays(1, &mVAOHandle);
        glBindVertexArray(mVAOHandle);

        mVertexBuffer->Bind();
        if (mHasIndex)
            mIndexBuffer->Bind();

        int count = 0;
        for (int i = 0, flag = 1; flag < LayoutName_Max; i++, flag <<= 1)
        {
            if (mLayoutFlag & flag)
            {
                glEnableVertexAttribArray(i);
                glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, totalStride, (void*)(count * mAttributeStride));
                ++count;
            }
        }

        glBindVertexArray(0);
        mDirty = false;
    }

    void StaticMesh::CalculateTBN()
    {
        // triangles must not share vertex, and should contain nromal and uv attribs
        if ((mLayoutFlag & LayoutName_Normal) && (mLayoutFlag & LayoutName_UV0))
        {
            for (size_t idx = 0; idx < mVertexCount; idx += 3)
            {
                auto &a = mPositions[idx];
                auto &b = mPositions[idx + 1];
                auto &c = mPositions[idx + 2];

                Eigen::Vector2f aUV = mUvs[0][idx].segment(0, 2);
                Eigen::Vector2f bUV = mUvs[0][idx + 1].segment(0, 2);
                Eigen::Vector2f cUV = mUvs[0][idx + 2].segment(0, 2);

                auto ab = b - a;
                auto ac = c - a;

                auto abUV = bUV - aUV;
                auto acUV = cUV - aUV;

                Eigen::Matrix<float, 3, 2> B;
                B.col(0) = ab;
                B.col(1) = ac;

                Eigen::Matrix2f A;
                A.col(0) = abUV;
                A.col(1) = acUV;

                Eigen::Matrix<float,2,2> inv;
                inv << A(1,1),-A(0,1),-A(1,0),A(0,0);
                inv /= (A(0,0)*A(1,1) - A(0,1)*A(1,0));
                // std::cout << A.inverse() << std::endl;
                // std::cout << inv << std::endl;

                auto TB = B * inv;
                mTangents.push_back(TB.col(0));
                mTangents.push_back(TB.col(0));
                mTangents.push_back(TB.col(0));

                mBiTangents.push_back(TB.col(1));
                mBiTangents.push_back(TB.col(1));
                mBiTangents.push_back(TB.col(1));
            }

            mLayoutFlag |= LayoutName_Tangent;
            mLayoutFlag |= LayoutName_Bitangent;
        }
    }

    void StaticMesh::Bind()
    {
        glBindVertexArray(mVAOHandle);
    }

    StaticMesh::~StaticMesh()
    {
        free(mPreparedBuffer);
        glDeleteVertexArrays(1, &mVAOHandle);
    }
}
