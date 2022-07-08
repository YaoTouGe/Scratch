/**
 * @file RenderPipeline.h
 * @author wangyudong
 * @brief A pipeline is used to organize the way to render objects, passes, and launch draw call.
 * @version 0.1
 * @date 2022-06-09
 * 
 */
#pragma once

#include <queue>
#include "Buffer.h"
#include "ShaderProgram.h"
#include "RenderPass.h"
#include "StaticMesh.h"
#include "RenderObject.h"

namespace RenderSystem
{
    struct Light
    {
        Eigen::Vector3f lightPos;
        int lightType; // 0 directional, 1 point light, 2 spot light
        Eigen::Vector3f lightColor;
        float intensity = 1.f;

        // parameters for different light types
    };

    class RenderPipeline
    {
    public:
        typedef std::shared_ptr<RenderPipeline> SP;

        RenderPipeline();
        ~RenderPipeline();

        void SetViewMatrix(const Eigen::Matrix4f &viewMat) { mGlobalData.viewMat = viewMat; }
        void SetProjectionMatrix(const Eigen::Matrix4f &projMat) { mGlobalData.projMat = projMat; }
        void SetCameraPos(const Eigen::Vector3f &pos) { mGlobalData.cameraPosition = pos; }

        // passes should be added only once.
        virtual void AddRenderPass(RenderPass::SP pass);
        // meshes should be collected each frame.
        virtual void CollectMesh(StaticMesh::SP mesh, Material::SP material, const Eigen::Matrix4f &modelMat);
        virtual void CollectLight(const Light& lightInfo);
        virtual void Submit();

        static const int maxLightCount = 16;

    private:
        struct GlobalUniformData
        {
            Eigen::Matrix4f viewMat;
            Eigen::Matrix4f projMat;
            Eigen::Matrix4f vpMat;
            Light lightData[maxLightCount]; // maximum 16 lights, for more lights maybe we should use deferred or forward+ shading.
            Eigen::Vector3f cameraPosition;
            int lightCount = 0;
        };

        void clear();

        std::vector<RenderPass::SP> mRenderPasses;
        std::vector<RenderObject> mRenderObjects;
        Buffer::SP mGlobalUniformBuffer;

        GlobalUniformData mGlobalData;
    };
}