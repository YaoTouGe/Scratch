#ifndef FRAMEWORK_APPS_RENDERSHOWCASE_H
#define FRAMEWORK_APPS_RENDERSHOWCASE_H

#include "Common/WindowApplication.h"
#include "StaticMesh.h"
#include "ShaderProgram.h"
#include "Material.h"
#include "Camera.h"
#include "RenderTexture.h"

namespace Application
{
    class RenderShowcase: public WindowApplication
    {
    public:
        RenderShowcase(const char *title, int width, int height);

    protected:
        virtual int Initialize() override;
        virtual void Finalize() override;
        virtual void Render() override;
        virtual void KeyBoard(int key, int action) override;
        virtual void MouseMove(int x, int y) override;
        virtual void MouseButton(int button, int action, int mods, int x, int y) override;

    private:
        void DrawAxis(const Eigen::Matrix4f &transform);

        Graphics::StaticMesh::SP mArrowMesh;
        Graphics::StaticMesh::SP mCubeMesh;
        Graphics::StaticMesh::SP mSphereMesh;

        Graphics::BasicPBRMaterial::SP mMaterialCopper;
        Graphics::BasicPBRMaterial::SP mMaterialBlock;
        Graphics::BasicPBRMaterial::SP mMaterialBlackWhite;

        Graphics::Texture::SP mAlbedo;
        Graphics::Texture::SP mNormal;
        Graphics::Texture::SP mMetallic;
        Graphics::Texture::SP mAo;
        Graphics::Texture::SP mRoughness;

        Graphics::ShaderProgram::SP mShaderProgram;

        Eigen::Matrix4f mViewMat;
        Eigen::Matrix4f mProjectionMat;

        Graphics::Camera *m_Camera;

        float mMetallicScale = 1;
        float mAOScale = 1;
        float mRoughnessScale = 1;

        Graphics::RenderTexture::SP mRT;
    };
}

#endif