#include "BasicMeshUtil.h"

namespace Graphics
{

    // Along y axis cylinder
    void GenUpwardCylinder(std::vector<Eigen::Vector3f> &verts, std::vector<Eigen::Vector3f> &normals, std::vector<uint32_t> &indices,
                           std::vector<Eigen::Vector3f> &uv, float bottomRadius, float upRadius, float length, float start)
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
            
            float s = (float)i / sec;
            uv.push_back(Eigen::Vector3f(s, 0, 0));
            uv.push_back(Eigen::Vector3f(s, 1, 0));

            if (i == sec - 1)
            {
                // replicate first tow vertices to separate uv
                verts.push_back(verts[indicesOffset]);
                verts.push_back(verts[indicesOffset + 1]);

                normals.push_back(normals[indicesOffset]);
                normals.push_back(normals[indicesOffset + 1]);

                uv.push_back(Eigen::Vector3f(1, 0, 0));
                uv.push_back(Eigen::Vector3f(1, 1, 0));
            }

            indices.push_back(i * 2 + indicesOffset);
            indices.push_back(i * 2 + 2 + indicesOffset);
            indices.push_back(i * 2 + 1 + indicesOffset);

            indices.push_back(i * 2 + 1 + indicesOffset);
            indices.push_back(i * 2 + 2 + indicesOffset);
            indices.push_back(i * 2 + 3 + indicesOffset);
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
                uv.push_back(Eigen::Vector3f(cosAngle * 0.5 + 0.5, sinAngle * 0.5 + 0.5, 0));
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
                uv.push_back(Eigen::Vector3f(cosAngle * 0.5 + 0.5, sinAngle * 0.5 + 0.5, 0));
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
        std::vector<Eigen::Vector3f> uv0;

        // body
        GenUpwardCylinder(verts, normals, indices, uv0, thickness, thickness, length, 0);
        // head
        GenUpwardCylinder(verts, normals, indices, uv0, arrowThickness, 0, arrowThickness * 2, length);

        auto ret  = std::make_shared<StaticMesh>();

        ret->SetPositions(std::move(verts));
        ret->SetNormals(std::move(normals));
        ret->SetIndices(std::move(indices));
        ret->SetUvs(uv0, 0);

        return ret;
    }
}