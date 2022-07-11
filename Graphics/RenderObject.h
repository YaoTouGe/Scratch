/**
 * @file RenderObject.h
 * @author wangyudong
 * @brief A render object represents a render unit in scene like static mesh (TODO: skinned mesh, particle system and so on).
 * @version 0.1
 * @date 2022-06-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include "VertexDataSource.h"
#include "Material.h"

namespace Graphics
{
    struct RenderObject
    {

    public:
        RenderObject(VertexDataSource::SP vertexSource, Material::SP material, const Eigen::Matrix4f &modelMatrix)
            : vertexSource(vertexSource), material(material) { objectData.modelMat = modelMatrix; }
        ~RenderObject() {}

        // Uniform buffer offset alignment is 256, more per object data in the future
        struct PerObjectData
        {
            Eigen::Matrix4f modelMat;
            Eigen::Matrix4f modelViewMat;
            Eigen::Matrix4f mvpMat;
            Eigen::Matrix4f padding2;
        };

        VertexDataSource::SP vertexSource;
        Material::SP material;
        PerObjectData objectData;

        const static size_t PerObjectDataSize = sizeof(PerObjectData);
    };
}