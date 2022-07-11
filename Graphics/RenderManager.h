/**
 * @file RenderManager.h
 * @author wangyudong
 * @brief Manage render states and graphics resources, encapsulate graphics APIs.
 * @version 0.1
 * @date 2022-06-07
 */

#pragma once

#include <queue>
#include "Texture.h"
#include "RenderTexture.h"
#include "Buffer.h"
#include "ShaderProgram.h"
#include "RenderPass.h"
#include "Material.h"
#include "StaticMesh.h"
#include "RenderPipeline.h"

namespace Graphics
{
    class RenderManager
    {
    public:
        static RenderManager *Instance();

        /****************** graphics resources ***********************/

        Buffer::SP AllocBuffer(BufferType type);
        Texture::SP AllocTexture() { return nullptr; }
        RenderTexture::SP AllocRenderTexture();
        ShaderProgram::SP AllocShaderProgram();

        void ReleaseBuffer(Buffer::SP buffer);
        void ReleaseShaderProgram(ShaderProgram::SP shaderProgram);

        void SetCurrentRenderTexture(RenderTexture::SP rt) {}
        RenderTexture::SP GetCurrentRenderTexture() { return mRenderTexture; }

        void BindBufferRange(Buffer::SP buffer, uint32_t bindPoint, uint32_t offset, uint32_t size);
        void BindBufferBase(Buffer::SP buffer, uint32_t bindPoint);

        /****************** draw functions ***********************/

        void DrawArrays(DrawType type, uint32_t first, uint32_t count);
        void DrawElements(DrawType type, uint32_t count);

        inline void DrawMesh(StaticMesh::SP mesh, Material::SP material, const Eigen::Matrix4f &modelMat) { mPipeline->CollectMesh(mesh, material, modelMat); }
        inline void AddRenderPass(RenderPass::SP pass) { mPipeline->AddRenderPass(pass); }

        /****************** other functions ***********************/

        void SetCurrentPipeline(RenderPipeline::SP pipe) { mPipeline = pipe; }
        RenderPipeline::SP GetCurrentPipeline() { return mPipeline; }

        void SetRenderStates(const RenderStates &states);
        void Clear(ClearFlag flag);
        void ClearColor(Eigen::Vector4f c);

        void SetViewMatrix(const Eigen::Matrix4f &viewMat) { mPipeline->SetViewMatrix(viewMat); }
        void SetProjectionMatrix(const Eigen::Matrix4f &projMat) { mPipeline->SetProjectionMatrix(projMat); }
        void SetCameraPos(const Eigen::Vector3f &pos) { mPipeline->SetCameraPos(pos); }

        void EnableWireFrame(bool enabled);
        void CollectLight(const Light &light) { mPipeline->CollectLight(light); }

        void EndFrame() { mPipeline->Submit(); }

    private:
        RenderManager();
        ~RenderManager() {}

        static RenderManager *mInstance;
        RenderPipeline::SP mPipeline;
        RenderTexture::SP mRenderTexture;
    };
}
