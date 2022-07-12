#include "RenderShowcase.h"
#include "RenderManager.h"
#include "RenderPipeline.h"
#include "ShaderUtil.h"
#include "BasicMeshUtil.h"

using namespace Graphics;

namespace Application
{
    RenderShowcase::RenderShowcase(const char *title, int width, int height)
        : WindowApplication(width, height, title)
    {
        m_Camera = new Camera(width, height);
        m_Camera->SetCamera(Eigen::Vector3d(0, 0, 0) /*lookat*/, Eigen::Vector3d(0, 0, 1) /*eye*/, Eigen::Vector3d(0, 1, 0) /*up*/);
    }

    int RenderShowcase::Initialize()
    {
        if (WindowApplication::Initialize() != 0)
            return -1;

        auto rp = std::make_shared<RenderPipeline>();
        RenderManager::Instance()->SetCurrentPipeline(rp);

        mArrowMesh = GenArrowMesh(0.02, 0.04, 1, false);
        mCubeMesh = GenCubeMesh(Eigen::Vector3f(1, 1, 1));

        mAlbedo = RenderManager::Instance()->AllocTexture(TextureType_2D, TextureFormat_R8G8B8, true);
        mAlbedo->SetFilter(TextureFilter_LinearMipmapLinear, TextureFilter_Linear);
        mAlbedo->SetWrapMode(TextureWrapMode_Repeat, TextureWrapMode_Repeat);
        mAlbedo->LoadFromFile("Resources/dull-copper_albedo.png");

        mNormal = RenderManager::Instance()->AllocTexture(TextureType_2D, TextureFormat_R8G8B8A8, true);
        mNormal->SetFilter(TextureFilter_LinearMipmapLinear, TextureFilter_Linear);
        mNormal->SetWrapMode(TextureWrapMode_Repeat, TextureWrapMode_Repeat);
        mNormal->LoadFromFile("Resources/dull-copper_normal-dx.png");

        mMetallic = RenderManager::Instance()->AllocTexture(TextureType_2D, TextureFormat_R8G8B8, true);
        mMetallic->SetFilter(TextureFilter_LinearMipmapLinear, TextureFilter_Linear);
        mMetallic->SetWrapMode(TextureWrapMode_Repeat, TextureWrapMode_Repeat);
        mMetallic->LoadFromFile("Resources/dull-copper_metallic.png");

        mAo = RenderManager::Instance()->AllocTexture(TextureType_2D, TextureFormat_R8G8B8A8, true);
        mAo->SetFilter(TextureFilter_LinearMipmapLinear, TextureFilter_Linear);
        mAo->SetWrapMode(TextureWrapMode_Repeat, TextureWrapMode_Repeat);
        mAo->LoadFromFile("Resources/dull-copper_ao.png");

        mRoughness = RenderManager::Instance()->AllocTexture(TextureType_2D, TextureFormat_R8G8B8, true);
        mRoughness->SetFilter(TextureFilter_LinearMipmapLinear, TextureFilter_Linear);
        mRoughness->SetWrapMode(TextureWrapMode_Repeat, TextureWrapMode_Repeat);
        mRoughness->LoadFromFile("Resources/dull-copper_roughness.png");

        mShaderProgram = ShaderUtil::LoadProgramFromTinySL("Graphics/shaders/basic_pbr.tinysl");
        mMaterialGreen = std::make_shared<Material>(mShaderProgram);
        mMaterialRed = std::make_shared<Material>(mShaderProgram);
        mMaterialBlue = std::make_shared<Material>(mShaderProgram);

        mMaterialRed->SetValue("mainColor", Eigen::Vector4f(1, 1, 1, 1));
        mMaterialRed->SetTexture("roughnessTex", mRoughness);
        mMaterialRed->SetTexture("metallicTex", mMetallic);
        mMaterialRed->SetTexture("albedoTex", mAlbedo);
        mMaterialRed->SetTexture("aoTex", mAo);
        mMaterialRed->SetTexture("normalTex", mNormal);

        mMaterialGreen->SetValue("mainColor", Eigen::Vector4f(1, 1, 1, 1));
        mMaterialGreen->SetTexture("roughnessTex", mRoughness);
        mMaterialGreen->SetTexture("metallicTex", mMetallic);
        mMaterialGreen->SetTexture("albedoTex", mAlbedo);
        mMaterialGreen->SetTexture("aoTex", mAo);
        mMaterialGreen->SetTexture("normalTex", mNormal);

        mMaterialBlue->SetValue("mainColor", Eigen::Vector4f(1, 1, 1, 1));
        mMaterialBlue->SetTexture("roughnessTex", mRoughness);
        mMaterialBlue->SetTexture("metallicTex", mMetallic);
        mMaterialBlue->SetTexture("albedoTex", mAlbedo);
        mMaterialBlue->SetTexture("aoTex", mAo);
        mMaterialBlue->SetTexture("normalTex", mNormal);
        return 0;
    }

    void RenderShowcase::Finalize()
    {
        WindowApplication::Finalize();
        RenderManager::Instance()->ReleaseShaderProgram(mShaderProgram);
        RenderManager::Instance()->ReleaseTexture(mAlbedo);
    }

    void RenderShowcase::Render()
    {
        auto rm = RenderManager::Instance();
        rm->Clear(ClearFlag_All);

        m_Camera->GetViewMatrix(mViewMat);
        m_Camera->GetProjectionMatrix(mProjectionMat);

        rm->SetViewMatrix(mViewMat);
        rm->SetProjectionMatrix(mProjectionMat);
        rm->SetCameraPos(m_Camera->GetEye().cast<float>());

        Light l;
        l.lightColor = Eigen::Vector3f(1, 1, 1);
        l.lightType = 0;
        l.lightPos = Eigen::Vector3f(1, 1, 1);
        l.intensity = 2;
        rm->CollectLight(l);
        l.lightPos = Eigen::Vector3f(-1, 1, 1);
        rm->CollectLight(l);
        l.lightPos = Eigen::Vector3f(1, -1, 1);
        rm->CollectLight(l);
        l.lightPos = Eigen::Vector3f(1, 1, -1);
        rm->CollectLight(l);

        DrawAxis(Eigen::Matrix4f::Identity());
        rm->DrawMesh(mCubeMesh, mMaterialBlue, Eigen::Matrix4f::Identity());
        // rm->EnableWireFrame(true);
        rm->EndFrame();
    }

    void RenderShowcase::KeyBoard(int key, int action)
    {
        // switch (key)
        // {
        // case GLFW_KEY_W:
        //     mMetallic += 0.05f;
        //     mMaterialRed->SetValue("metallic", mMetallic);
        //     mMaterialGreen->SetValue("metallic", mMetallic);
        //     mMaterialBlue->SetValue("metallic", mMetallic);
        //     break;

        // case GLFW_KEY_S:
        //     mMetallic -= 0.05f;
        //     mMaterialRed->SetValue("metallic", mMetallic);
        //     mMaterialGreen->SetValue("metallic", mMetallic);
        //     mMaterialBlue->SetValue("metallic", mMetallic);
        //     break;
        // case GLFW_KEY_A:
        //     mRoughness -= 0.05f;
        //     mMaterialRed->SetValue("roughness", mRoughness);
        //     mMaterialGreen->SetValue("roughness", mRoughness);
        //     mMaterialBlue->SetValue("roughness", mRoughness);
        //     break;
        // case GLFW_KEY_D:
        //     mRoughness += 0.05f;
        //     mMaterialRed->SetValue("roughness", mRoughness);
        //     mMaterialGreen->SetValue("roughness", mRoughness);
        //     mMaterialBlue->SetValue("roughness", mRoughness);
        //     break;
        // }
    }

    void RenderShowcase::DrawAxis(const Eigen::Matrix4f &transform)
    {
        auto rm = RenderManager::Instance();
        Eigen::AngleAxisf rotation(M_PI_2, Eigen::Vector3f(0, 0, -1));
        Eigen::Affine3f local;
        local.setIdentity();
        local.linear() = rotation.toRotationMatrix();

        rm->DrawMesh(mArrowMesh, mMaterialRed, transform * local.matrix());
        rm->DrawMesh(mArrowMesh, mMaterialGreen, transform);

        rotation.axis() = Eigen::Vector3f(1, 0, 0);
        local.linear() = rotation.toRotationMatrix();
        rm->DrawMesh(mArrowMesh, mMaterialBlue, transform * local.matrix());
    }

    void RenderShowcase::MouseButton(int button, int action, int mods, int x, int y)
    {
        if (action == GLFW_PRESS)
        {
            m_IsDrag = true;
            m_MouseType = button;
            m_PrevX = x;
            m_PrevY = y;
        }
        else
        {
            m_IsDrag = false;
            m_MouseType = -1;
        }
    }

    void RenderShowcase::MouseMove(int x, int y)
    {
        if (m_MouseType == GLFW_MOUSE_BUTTON_LEFT)
        {
            m_Camera->Translate(x, y, m_PrevX, m_PrevY);
        }
        else if (m_MouseType == GLFW_MOUSE_BUTTON_MIDDLE)
        {
            m_Camera->Pan(x, y, m_PrevX, m_PrevY);
        }
        else if (m_MouseType == GLFW_MOUSE_BUTTON_RIGHT)
        {
            m_Camera->Rotate(x, y, m_PrevX, m_PrevY);
        }
        m_PrevX = x;
        m_PrevY = y;
    }
    
    RenderShowcase showCase("Render Showcase", 1280, 720);
    IApplication *g_app = &showCase;
}