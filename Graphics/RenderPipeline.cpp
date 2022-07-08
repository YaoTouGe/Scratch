#include "RenderPipeline.h"
#include "RenderManager.h"
#include "InternalFunctions.h"

namespace RenderSystem
{
    RenderPipeline::RenderPipeline()
    {
        mGlobalUniformBuffer = RenderManager::Instance()->AllocBuffer(BufferType_UniformBuffer);
    }

    RenderPipeline::~RenderPipeline()
    {
        RenderManager::Instance()->ReleaseBuffer(mGlobalUniformBuffer);
    }

    void RenderPipeline::AddRenderPass(RenderPass::SP renderPass)
    {
        renderPass->SetUp();
        mRenderPasses.push_back(renderPass);
    }

    void RenderPipeline::CollectMesh(StaticMesh::SP mesh, Material::SP material, const Eigen::Matrix4f &modelMat)
    {
        mRenderObjects.push_back(RenderObject(mesh, material, modelMat));
    }

    void RenderPipeline::Submit()
    {
        // calculate frequently used Matrix.
        mGlobalData.vpMat = mGlobalData.projMat * mGlobalData.viewMat;

        mGlobalUniformBuffer->BufferData(&mGlobalData, sizeof(GlobalUniformData), BufferUsage_StaticDraw);
        auto rm = RenderManager::Instance();
        auto perObjectUbo = rm->AllocBuffer(BufferType_UniformBuffer);

        // currently per-object data only contains transform
        size_t perObjectBufferSize = RenderObject::PerObjectDataSize * mRenderObjects.size();
        auto perObjectBuffer = malloc(perObjectBufferSize);

        auto writePtr = perObjectBuffer;
        // prepare and collect per-object data
        for (size_t i = 0; i < mRenderObjects.size(); ++i)
        {
            auto &ro = mRenderObjects[i];
            ro.vertexSource->Prepare();
            ro.material->Prepare();
            ro.objectData.mvpMat = mGlobalData.vpMat * ro.objectData.modelMat;
            ro.objectData.modelViewMat = mGlobalData.viewMat * ro.objectData.modelMat;
            memcpy(writePtr, &ro.objectData, RenderObject::PerObjectDataSize);
            writePtr = (void *)((char *)writePtr + RenderObject::PerObjectDataSize);
        }

        perObjectUbo->BufferData(perObjectBuffer, perObjectBufferSize, BufferUsage_StaticDraw);
        rm->BindBufferBase(mGlobalUniformBuffer, GlobalUBOBindPoint);
        
        // draw render objects
        for (size_t i = 0; i < mRenderObjects.size(); ++i)
        {
            auto &ro = mRenderObjects[i];
            ro.vertexSource->Bind();
            ro.material->Use();
            ro.material->SetStates();
            rm->BindBufferRange(perObjectUbo, PerObjectUBOBindPoint, i * RenderObject::PerObjectDataSize, RenderObject::PerObjectDataSize);

            if (ro.vertexSource->HasIndex())
                rm->DrawElements(DrawType_Triangles, ro.vertexSource->IndexCount());
            else
                rm->DrawArrays(DrawType_Triangles, 0, ro.vertexSource->VertexCount());
        }

        rm->ReleaseBuffer(perObjectUbo);

        clear();
    }

    void RenderPipeline::clear()
    {
        mRenderObjects.clear();
        mGlobalData.lightCount = 0;
    }

    void RenderPipeline::CollectLight(const Light &lightInfo)
    {
        if (mGlobalData.lightCount >= maxLightCount)
        {
            RS_LOG_ERROR("Too many lights!");
            return;
        }

        mGlobalData.lightData[mGlobalData.lightCount++] = lightInfo;
    }
}
