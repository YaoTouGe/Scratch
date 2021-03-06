#include "Material.h"
#include "RenderManager.h"
#include "GL/glew.h"

namespace Graphics
{
    Material::Material(ShaderProgram::SP shader)
    {
        mMaterialUniformBuffer = RenderManager::Instance()->AllocBuffer(BufferType_UniformBuffer);
        mShader = shader;
        auto propertyLayout = mShader->GetPropertyLayout();
        // propertyLayout->PrintLayoutInfos();

        // only recognize PerMaterial UBO currently, support arbitary UBO later.
        for (size_t i = 0; i < propertyLayout->uniformBlockInfos.size(); ++i)
        {
            auto &blockInfo = propertyLayout->uniformBlockInfos[i];
            if (blockInfo.blockName.compare(PerMaterialUBOName) == 0)
            {
                mPerMaterialBufferSize = blockInfo.blockSize;
                mPerMaterialBuffer = malloc(mPerMaterialBufferSize);

                for (size_t j = 0; j < blockInfo.uniformNames.size(); ++j)
                {
                    mPerMaterialBlockOffset.insert(std::make_pair(blockInfo.uniformNames[j], blockInfo.uniformOffset[j]));
                }
                break;
            }
        }

        // out of block uniforms
        for (size_t i = 0; i < propertyLayout->uniformInfos.size(); ++i)
        {
            auto &info = propertyLayout->uniformInfos[i];
            mUniformCaches.insert(std::make_pair(info.name, UniformCache(info.type, info.location)));
        }
    }

    Material::~Material()
    {
        free(mPerMaterialBuffer);
        for(auto p : mUniformCaches)
        {
            free(p.second.data);
        }
    }

    void Material::Prepare()
    {
        // upload UBO
        if (!mDirty)
            return;
        mMaterialUniformBuffer->BufferData(mPerMaterialBuffer, mPerMaterialBufferSize, BufferUsage_StaticDraw);

        mShader->UseProgram();
        for (auto &c : mUniformCaches)
        {
            if (c.second.data == nullptr)
                continue;

            switch (c.second.type)
            {
            case ProgramDataType_Float:
                glUniform1fv(c.second.location, 1, reinterpret_cast<GLfloat *>(&c.second.data));
                break;
            case ProgramDataType_Int:
                glUniform1iv(c.second.location, 1, reinterpret_cast<GLint *>(&c.second.data));
                break;
            case ProgramDataType_Vec2:
                glUniform2fv(c.second.location, 1, (GLfloat *)c.second.data);
                break;
            case ProgramDataType_Vec3:
                glUniform3fv(c.second.location, 1, (GLfloat *)c.second.data);
                break;
            case ProgramDataType_Vec4:
                glUniform4fv(c.second.location, 1, (GLfloat *)c.second.data);
                break;
            case ProgramDataType_Mat4:
                glUniformMatrix4fv(c.second.location, 1, GL_FALSE, (GLfloat *)c.second.data);
                break;
            }
        }
        mDirty = false;
    }

    void Material::Use()
    {
        mShader->UseProgram();
        RenderManager::Instance()->BindBufferBase(mMaterialUniformBuffer, PerMaterialUBOBindPoint);

        auto samplerInfos = mShader->GetSamplerInfos();
        for (int texUnit = 0; texUnit < samplerInfos.size(); ++texUnit)
        {
            auto iter = mTextureBinds.find(samplerInfos[texUnit].name);
            if (iter != mTextureBinds.end())
            {
                glActiveTexture(texUnit + GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, iter->second);
            }
        }
    }

    void Material::SetStates()
    {
        RenderManager::Instance()->SetRenderStates(mShader->GetStates());
    }

    BasicPBRMaterial::BasicPBRMaterial(ShaderProgram::SP shader) : Material(shader)
    {
        SetRoughnessScale(1);
        SetMetallicScale(1);
        SetAOScale(1);
    }

    BasicPBRMaterial::~BasicPBRMaterial()
    {
    }

    void BasicPBRMaterial::LoadTextures(const char *texPaths[5], TextureFormat formats[5])
    {
        const char *texNames[5] = {"albedoTex","metallicTex", "roughnessTex", "aoTex", "normalTex"};
        for (int i = 0; i < 5; ++i)
        {
            if (texPaths[i] == nullptr)
            {
                mTextures[i] = Texture::GetWhiteTexture();
            }
            else
            {
                auto tex = RenderManager::Instance()->AllocTexture(TextureType_2D, formats[i], true);
                tex->SetFilter(TextureFilter_LinearMipmapLinear, TextureFilter_Linear);
                tex->SetWrapMode(TextureWrapMode_Repeat, TextureWrapMode_Repeat);
                tex->LoadFromFile(texPaths[i]);
                mTextures[i] = tex;
            }
            SetTexture(texNames[i], mTextures[i]);
        }
    }
}
