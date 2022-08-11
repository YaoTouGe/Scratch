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

        mArrowMesh = GenArrowMesh(0.02f, 0.04f, 1, false);
        mCubeMesh = GenCubeMesh(Eigen::Vector3f(0.2f, 0.2f, 0.2f));
        mSphereMesh = GenSphereMesh(0.2f, 50);
        // mSphereMesh = GenCubeMesh(Eigen::Vector3f(0.2, 0.2, 0.2));

        mAlbedo = RenderManager::Instance()->AllocTexture(TextureType_2D, TextureFormat_R8G8B8, true);
        mAlbedo->SetFilter(TextureFilter_LinearMipmapLinear, TextureFilter_Linear);
        mAlbedo->SetWrapMode(TextureWrapMode_Repeat, TextureWrapMode_Repeat);
        mAlbedo->LoadFromFile("Resources/copper/dull-copper_albedo.png");

        mNormal = RenderManager::Instance()->AllocTexture(TextureType_2D, TextureFormat_R8G8B8A8, true);
        mNormal->SetFilter(TextureFilter_LinearMipmapLinear, TextureFilter_Linear);
        mNormal->SetWrapMode(TextureWrapMode_Repeat, TextureWrapMode_Repeat);
        mNormal->LoadFromFile("Resources/copper/dull-copper_normal-dx.png");

        mMetallic = RenderManager::Instance()->AllocTexture(TextureType_2D, TextureFormat_R8G8B8, true);
        mMetallic->SetFilter(TextureFilter_LinearMipmapLinear, TextureFilter_Linear);
        mMetallic->SetWrapMode(TextureWrapMode_Repeat, TextureWrapMode_Repeat);
        mMetallic->LoadFromFile("Resources/copper/dull-copper_metallic.png");

        mAo = RenderManager::Instance()->AllocTexture(TextureType_2D, TextureFormat_R8G8B8A8, true);
        mAo->SetFilter(TextureFilter_LinearMipmapLinear, TextureFilter_Linear);
        mAo->SetWrapMode(TextureWrapMode_Repeat, TextureWrapMode_Repeat);
        mAo->LoadFromFile("Resources/copper/dull-copper_ao.png");

        mRoughness = RenderManager::Instance()->AllocTexture(TextureType_2D, TextureFormat_R8G8B8, true);
        mRoughness->SetFilter(TextureFilter_LinearMipmapLinear, TextureFilter_Linear);
        mRoughness->SetWrapMode(TextureWrapMode_Repeat, TextureWrapMode_Repeat);
        mRoughness->LoadFromFile("Resources/copper/dull-copper_roughness.png");

        mShaderProgram = ShaderUtil::LoadProgramFromTinySL("Graphics/shaders/basic_pbr.tinysl");
        mMaterialCopper = std::make_shared<BasicPBRMaterial>(mShaderProgram);
        mMaterialBlock = std::make_shared<BasicPBRMaterial>(mShaderProgram);
        mMaterialBlackWhite = std::make_shared<BasicPBRMaterial>(mShaderProgram);

        mMaterialBlock->SetValue("mainColor", Eigen::Vector4f(1, 1, 1, 1));
        const char *texs[5] = {"Resources/cobble/dusty-cobble_albedo.png",
                               "Resources/cobble/dusty-cobble_metallic.png",
                               "Resources/cobble/dusty-cobble_roughness.png",
                               "Resources/cobble/dusty-cobble_ao.png",
                               "Resources/cobble/dusty-cobble_normal-ogl.png"};
        TextureFormat formats[5] = {
            TextureFormat_R8G8B8,
            TextureFormat_R8G8B8,
            TextureFormat_R8G8B8,
            TextureFormat_R8G8B8,
            TextureFormat_R8G8B8
        };
        mMaterialBlock->LoadTextures(texs, formats);

        mMaterialCopper->SetValue("mainColor", Eigen::Vector4f(1, 1, 1, 1));
        mMaterialCopper->SetTexture("roughnessTex", mRoughness);
        mMaterialCopper->SetTexture("metallicTex", mMetallic);
        mMaterialCopper->SetTexture("albedoTex", mAlbedo);
        mMaterialCopper->SetTexture("aoTex", mAo);
        mMaterialCopper->SetTexture("normalTex", mNormal);

        mMaterialBlackWhite->SetValue("mainColor", Eigen::Vector4f(1, 1, 1, 1));
        const char *blackWhiteTexs[5] = {
            "Resources/black_white/black-white-tile_albedo.png",
            "Resources/black_white/black-white-tile_metallic.png",
            nullptr,
            "Resources/black_white/black-white-tile_ao.png",
            "Resources/black_white/black-white-tile_normal-ogl.png"};

        TextureFormat blackWhiteFormats[5] = {
            TextureFormat_R8G8B8A8,
            TextureFormat_R8G8,
            TextureFormat_R8G8B8,
            TextureFormat_R8G8B8,
            TextureFormat_R8G8B8};
        mMaterialBlackWhite->LoadTextures(blackWhiteTexs, blackWhiteFormats);
        return 0;
    }

    void RenderShowcase::Finalize()
    {
        WindowApplication::Finalize();
    }

    void RenderShowcase::Render()
    {
        auto rm = RenderManager::Instance();
        rm->Clear(ClearFlag_All);
        // rm->EnableWireFrame(true);

        m_Camera->GetViewMatrix(mViewMat);
        m_Camera->GetProjectionMatrix(mProjectionMat);

        rm->SetViewMatrix(mViewMat);
        rm->SetProjectionMatrix(mProjectionMat);
        rm->SetCameraPos(m_Camera->GetEye().cast<float>());

        Light l;
        l.lightColor = Eigen::Vector3f(1, 1, 1);
        l.lightType = 0;
        l.lightPos = Eigen::Vector3f(1, 1, 1);
        l.intensity = 1;
        rm->CollectLight(l);
        l.lightPos = Eigen::Vector3f(-1, 1, 1);
        rm->CollectLight(l);
        l.lightPos = Eigen::Vector3f(1, -1, 1);
        rm->CollectLight(l);
        l.lightPos = Eigen::Vector3f(1, 1, -1);
        rm->CollectLight(l);

        DrawAxis(Eigen::Matrix4f::Identity());
        Eigen::Affine3f transform;
        transform.setIdentity();
        // transform.translation() = Eigen::Vector3f(0, 1, 0);
        rm->DrawMesh(mCubeMesh, mMaterialBlackWhite, transform.matrix());

        transform.setIdentity();
        transform.translation() = Eigen::Vector3f(0, 0, 0.5);
        rm->DrawMesh(mSphereMesh, mMaterialBlackWhite, transform.matrix());
        // rm->EnableWireFrame(true);
        rm->EndFrame();
    }

    void RenderShowcase::KeyBoard(int key, int action)
    {
        switch (key)
        {
        case GLFW_KEY_W:
            mMetallicScale += 0.05f;
            mMaterialBlock->SetMetallicScale(mMetallicScale);
            mMaterialCopper->SetMetallicScale(mMetallicScale);
            mMaterialBlackWhite->SetMetallicScale(mMetallicScale);
            break;

        case GLFW_KEY_S:
            mMetallicScale -= 0.05f;
            mMaterialBlock->SetMetallicScale(mMetallicScale);
            mMaterialCopper->SetMetallicScale(mMetallicScale);
            mMaterialBlackWhite->SetMetallicScale(mMetallicScale);
            break;
        case GLFW_KEY_A:
            mRoughnessScale -= 0.05f;
            mMaterialBlock->SetRoughnessScale(mRoughnessScale);
            mMaterialCopper->SetRoughnessScale(mRoughnessScale);
            mMaterialBlackWhite->SetRoughnessScale(mRoughnessScale);
            break;
        case GLFW_KEY_D:
            mRoughnessScale += 0.05f;
            mMaterialBlock->SetRoughnessScale(mRoughnessScale);
            mMaterialCopper->SetRoughnessScale(mRoughnessScale);
            mMaterialBlackWhite->SetRoughnessScale(mRoughnessScale);
            break;
        }
    }

    void RenderShowcase::DrawAxis(const Eigen::Matrix4f &transform)
    {
        auto rm = RenderManager::Instance();
        Eigen::AngleAxisf rotation(PI_2_F, Eigen::Vector3f(0, 0, -1));
        Eigen::Affine3f local;
        local.setIdentity();
        local.linear() = rotation.toRotationMatrix();

        rm->DrawMesh(mArrowMesh, mMaterialCopper, transform * local.matrix());
        rm->DrawMesh(mArrowMesh, mMaterialCopper, transform);

        rotation.axis() = Eigen::Vector3f(1, 0, 0);
        local.linear() = rotation.toRotationMatrix();
        rm->DrawMesh(mArrowMesh, mMaterialCopper, transform * local.matrix());
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