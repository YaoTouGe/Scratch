#include "StaticMesh.h"
#include "RenderManager.h"
#include "GL/glew.h"

namespace Graphics
{
    void StaticMesh::Prepare()
    {
        if (!mDirty)
            return;
            
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
        std::vector<Eigen::Vector3f> *attribs[] = {&mPositions, &mNormals, &mUvs[0], &mUvs[1], &mUvs[2], &mColors[0], &mColors[1], &mColors[2]};

        for (int i = 0, flag = 1; flag < LayoutName_Max; i++, flag <<= 1)
        {
            if (mLayoutFlag & flag)
            {
                auto writePtr = (float *)writeStart;
                for (int j = 0; j < attribs[i]->size(); ++j)
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
        mVertexCount = mPositions.size();
        mIndexCount = mIndices.size();
        mDirty = false;
    }

    void StaticMesh::Bind()
    {
        glBindVertexArray(mVAOHandle);
    }

    StaticMesh::~StaticMesh()
    {
        free(mPreparedBuffer);
        RenderManager::Instance()->ReleaseBuffer(mVertexBuffer);
        RenderManager::Instance()->ReleaseBuffer(mIndexBuffer);
        glDeleteVertexArrays(1, &mVAOHandle);
    }
}
