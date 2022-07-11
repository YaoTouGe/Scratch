/**
 * @file Material.h
 * @author wangyudong
 * @brief A material is a combination of shader program and its render data like texture, uniforms and so on (may contain pipeline states later).
 * @version 0.1
 * @date 2022-06-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <memory>
#include <unordered_map>
#include "Eigen/Core"
#include "ShaderProgram.h"
#include "Buffer.h"

namespace Graphics
{
    class Material
    {
    public:
        typedef std::shared_ptr<Material> SP;

        Material(ShaderProgram::SP shader);
        ~Material();

        // Values set here are cached in main memory. When material is used for drawing, things will be uploaded
        // to GPU memroy if UBO or uniforms are dirty. There are 2 types material uniform data:
        //  1. Enclosed in uniform buffer block (recommended) will be uploaded as UBO.
        //  2. Out of uniform block, needs upload saperately.
        template<typename T>
        void SetValue(const std::string &name, const T &value)
        {
            auto iter = mPerMaterialBlockOffset.find(name);
            if (iter != mPerMaterialBlockOffset.end())
            {

                assert(sizeof(T) + iter->second <= mPerMaterialBufferSize);
                *reinterpret_cast<T *>((char*)mPerMaterialBuffer + iter->second) = value;
                mDirty = true;
            }
            else
            {
                // out of uniform block
                auto iterUniform = mUniformCaches.find(name);
                if (iterUniform == mUniformCaches.end())
                {
                    RS_LOG_ERROR_FMT("Set a undefined uniform: %s", name.c_str());
                    return;
                }
                // TODO: use mempool later
                iterUniform->second.data = malloc(sizeof(T));
                *reinterpret_cast<T *>(iterUniform->second.data) = value;
                mDirty = true;
            }
        }

        int GetPriority() { return mPriority; }
        
        // upload per material uniform data
        void Prepare();
        // bind uniform buffer and shader program.
        void Use();
        // set states of shader program
        void SetStates();

    protected:

        // lower is higher
        int mPriority = 0;
        bool mDirty = true;
        ShaderProgram::SP mShader;
        Buffer::SP mMaterialUniformBuffer;

        void* mPerMaterialBuffer = nullptr;
        size_t mPerMaterialBufferSize = 0;

        // cache out of block uniforms
        struct UniformCache
        {
            UniformCache(ProgramDataType type, int loc)
            {
                this->type = type;
                this->location = loc;
                this->data = nullptr;
            }
            ProgramDataType type;
            int location;
            void* data;
        };

        std::unordered_map<std::string, UniformCache> mUniformCaches;
        std::unordered_map<std::string, int> mPerMaterialBlockOffset;
    };
}
