#ifndef FRAMEWORK_APPS_RENDERSHOWCASE_H
#define FRAMEWORK_APPS_RENDERSHOWCASE_H

#include "WindowApplication.h"
#include "StaticMesh.h"
#include "ShaderProgram.h"
#include "Material.h"
#include "Camera.h"

namespace Application
{
    class RenderShowcase: public WindowApplication
    {
    public:
        RenderShowcase(const char *title, int width, int height);

    protected:
        virtual int Initialize() override;
        virtual void Render() override;
        virtual void KeyBoard(int key, int action) override;
        virtual void MouseMove(int x, int y) override;
        virtual void MouseButton(int button, int action, int mods, int x, int y) override;

    private:
        void DrawAxis(const Eigen::Matrix4f &transform);

        Graphics::StaticMesh::SP mArrowMesh;
        Graphics::Material::SP mMaterialGreen;
        Graphics::Material::SP mMaterialRed;
        Graphics::Material::SP mMaterialBlue;

        Eigen::Matrix4f mViewMat;
        Eigen::Matrix4f mProjectionMat;

        float mMetallic = 0;
        float mRoughness = 1;
        Graphics::Camera *m_Camera;
    };
}

#endif