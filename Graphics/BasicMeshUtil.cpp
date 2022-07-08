#include "BasicMeshUtil.h"

namespace RenderSystem
{

    // Along y axis cylinder
    void GenUpwardCylinder(std::vector<Eigen::Vector3f> &verts, std::vector<Eigen::Vector3f> &normals, std::vector<uint32_t> &indices,
                           float bottomRadius, float upRadius, float length, float start)
    {
        const int sec = 16;
        const float angleDelta = 2 * M_PI / sec;
        float angle = 0;
        float deltaRadius = bottomRadius - upRadius;
        float slopeLen = sqrtf(deltaRadius * deltaRadius + length * length);
        float sinSlope = deltaRadius / slopeLen;
        float cosSlope = length / slopeLen;

        size_t indicesOffset = verts.size();
        for (int i = 0; i < sec; ++i)
        {
            Eigen::Vector3f bottom;
            Eigen::Vector3f up;
            float sinAngle = sin(angle);
            float cosAngle = cos(angle);

            bottom = Eigen::Vector3f(sinAngle * bottomRadius, start, cosAngle * bottomRadius);
            up = Eigen::Vector3f(sinAngle * upRadius, start + length, cosAngle * upRadius);

            verts.push_back(bottom);
            verts.push_back(up);

            Eigen::Vector3f normal = Eigen::Vector3f(cosSlope * sinAngle, sinSlope, cosSlope * cosAngle);
            normals.push_back(normal * (bottomRadius == 0 ? 0 : 1));
            normals.push_back(normal * (upRadius == 0 ? 0 : 1));

            if (i != sec - 1)
            {
                indices.push_back(i * 2 + indicesOffset);
                indices.push_back(i * 2 + 2 + indicesOffset);
                indices.push_back(i * 2 + 1 + indicesOffset);

                indices.push_back(i * 2 + 1 + indicesOffset);
                indices.push_back(i * 2 + 2 + indicesOffset);
                indices.push_back(i * 2 + 3 + indicesOffset);
            }
            else
            {
                indices.push_back(i * 2 + 1 + indicesOffset);
                indices.push_back(i * 2 + indicesOffset);
                indices.push_back(0 + indicesOffset);

                indices.push_back(i * 2 + 1 + indicesOffset);
                indices.push_back(0 + indicesOffset);
                indices.push_back(1 + indicesOffset);
            }

            angle += angleDelta;
        }

        if (bottomRadius != 0)
        {
            indicesOffset = verts.size();
            angle = 0;
            for (int i = 0; i < sec; ++i, angle += angleDelta)
            {
                float sinAngle = sin(angle);
                float cosAngle = cos(angle);

                verts.push_back(Eigen::Vector3f(sinAngle * bottomRadius, start, cosAngle * bottomRadius));
                normals.push_back(Eigen::Vector3f(0, -1, 0));
            }

            for (int i = 1; i < sec - 1; ++i)
            {
                indices.push_back(i + indicesOffset);
                indices.push_back(0 + indicesOffset);
                indices.push_back(i + 1 + indicesOffset);
            }
        }

        if (upRadius != 0)
        {
            angle = 0;
            indicesOffset = verts.size();
            for (int i = 0; i < sec; ++i, angle += angleDelta)
            {
                float sinAngle = sin(angle);
                float cosAngle = cos(angle);

                verts.push_back(Eigen::Vector3f(sinAngle * upRadius, start + length, cosAngle * upRadius));
                normals.push_back(Eigen::Vector3f(0, 1, 0));
            }

            for (int i = 1; i < sec - 1; ++i)
            {
                indices.push_back(0 + indicesOffset);
                indices.push_back(i + indicesOffset);
                indices.push_back(i + 1 + indicesOffset);
            }
        }
    }

    StaticMesh::SP GenArrowMesh(float thickness, float arrowThickness, float length)
    {
        std::vector<Eigen::Vector3f> verts;
        std::vector<Eigen::Vector3f> normals;
        std::vector<uint32_t> indices;

        // body
        GenUpwardCylinder(verts, normals, indices, thickness, thickness, length, 0);
        // head
        GenUpwardCylinder(verts, normals, indices, arrowThickness, 0, arrowThickness * 2, length);

        auto ret  = std::make_shared<StaticMesh>();

        ret->SetPositions(std::move(verts));
        ret->SetNormals(std::move(normals));
        ret->SetIndices(std::move(indices));

        return ret;
    }
}