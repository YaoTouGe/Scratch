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
        RenderManager::Instance()->ReleaseBuffer(mMaterialUniformBuffer);
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
        mDirty = false;
    }

    void Material::Use()
    {
        mShader->UseProgram();
        RenderManager::Instance()->BindBufferBase(mMaterialUniformBuffer, PerMaterialUBOBindPoint);
        for(auto& c : mUniformCaches)
        {
            if (c.second.data == nullptr)
                continue;

            switch (c.second.type)
            {
            case ProgramDataType_Float:
                glUniform1fv(c.second.location, 1, (GLfloat*)c.second.data);
                break;
            case ProgramDataType_Int:
                glUniform1iv(c.second.location, 1, (GLint *)c.second.data);
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
    }

    void Material::SetStates()
    {
        RenderManager::Instance()->SetRenderStates(mShader->GetStates());
    }
}
