#include <iostream>
#include "BasicMeshUtil.h"

namespace Graphics
{

    // Along y axis cylinder
    void GenUpwardCylinder(std::vector<Eigen::Vector3f> &verts, std::vector<Eigen::Vector3f> &normals, std::vector<uint32_t> &indices,
                           std::vector<Eigen::Vector3f> &uv, float bottomRadius, float upRadius, float length, float start)
    {
        const int sec = 16;
        const float angleDelta = 2 * PI_F / sec;
        float angle = 0;
        float deltaRadius = bottomRadius - upRadius;
        float slopeLen = sqrtf(deltaRadius * deltaRadius + length * length);
        float sinSlope = deltaRadius / slopeLen;
        float cosSlope = length / slopeLen;

        uint32_t indicesOffset = (uint32_t)verts.size();
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
            indicesOffset = (uint32_t)verts.size();
            angle = 0;
            for (int i = 0; i < sec; ++i, angle += angleDelta)
            {
                float sinAngle = sin(angle);
                float cosAngle = cos(angle);

                verts.push_back(Eigen::Vector3f(sinAngle * bottomRadius, start, cosAngle * bottomRadius));
                normals.push_back(Eigen::Vector3f(0, -1, 0));
                uv.push_back(Eigen::Vector3f(cosAngle * 0.5f + 0.5f, sinAngle * 0.5f + 0.5f, 0));
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
            indicesOffset = (uint32_t)verts.size();
            for (int i = 0; i < sec; ++i, angle += angleDelta)
            {
                float sinAngle = sin(angle);
                float cosAngle = cos(angle);

                verts.push_back(Eigen::Vector3f(sinAngle * upRadius, start + length, cosAngle * upRadius));
                normals.push_back(Eigen::Vector3f(0, 1, 0));
                uv.push_back(Eigen::Vector3f(cosAngle * 0.5f + 0.5f, sinAngle * 0.5f + 0.5f, 0));
            }

            for (int i = 1; i < sec - 1; ++i)
            {
                indices.push_back(0 + indicesOffset);
                indices.push_back(i + indicesOffset);
                indices.push_back(i + 1 + indicesOffset);
            }
        }
    }
    // no indices
    void GenUpwardCylinder(std::vector<Eigen::Vector3f> &verts, std::vector<Eigen::Vector3f> &normals,
                           std::vector<Eigen::Vector3f> &uv, float bottomRadius, float upRadius, float length, float start)
    {
        const int sec = 16;
        const float angleDelta = 2 * PI_F / sec;
        float angle = 0;
        float deltaRadius = bottomRadius - upRadius;
        float slopeLen = sqrtf(deltaRadius * deltaRadius + length * length);
        float sinSlope = deltaRadius / slopeLen;
        float cosSlope = length / slopeLen;

        Eigen::Vector3f lastBottom;
        float lastS;
        Eigen::Vector3f lastNormal;

        size_t indicesOffset = verts.size();
        for (int i = 0; i < sec; ++i)
        {
            Eigen::Vector3f bottom;
            Eigen::Vector3f up;
            float sinAngle = sin(angle);
            float cosAngle = cos(angle);

            bottom = Eigen::Vector3f(sinAngle * bottomRadius, start, cosAngle * bottomRadius);
            up = Eigen::Vector3f(sinAngle * upRadius, start + length, cosAngle * upRadius);
            Eigen::Vector3f normal = Eigen::Vector3f(cosSlope * sinAngle, sinSlope, cosSlope * cosAngle);
            float s = (float)i / sec;

            if (i != 0)
            {
                verts.push_back(up);
                normals.push_back(normal * (upRadius == 0 ? 0 : 1));
                uv.push_back(Eigen::Vector3f(s, 1, 0));

                verts.push_back(up);
                normals.push_back(normal * (upRadius == 0 ? 0 : 1));
                uv.push_back(Eigen::Vector3f(s, 1, 0));

                verts.push_back(lastBottom);
                normals.push_back(lastNormal * (bottomRadius == 0 ? 0 : 1));
                uv.push_back(Eigen::Vector3f(lastS, 0, 0));

                verts.push_back(bottom);
                normals.push_back(normal * (bottomRadius == 0 ? 0 : 1));
                uv.push_back(Eigen::Vector3f(s, 0, 0));
            }

            verts.push_back(up);
            normals.push_back(normal * (upRadius == 0 ? 0 : 1));
            uv.push_back(Eigen::Vector3f(s, 1, 0));

            verts.push_back(bottom);
            normals.push_back(normal * (bottomRadius == 0 ? 0 : 1));
            uv.push_back(Eigen::Vector3f(s, 0, 0));

            if (i == sec - 1)
            {
                verts.push_back(verts[indicesOffset]);
                normals.push_back(normals[indicesOffset]);
                uv.push_back(Eigen::Vector3f(1, 1, 0));

                verts.push_back(verts[indicesOffset]);
                normals.push_back(normals[indicesOffset]);
                uv.push_back(Eigen::Vector3f(1, 1, 0));

                verts.push_back(bottom);
                normals.push_back(normal * (bottomRadius == 0 ? 0 : 1));
                uv.push_back(Eigen::Vector3f(s, 0, 0));

                verts.push_back(verts[indicesOffset + 1]);
                normals.push_back(normals[indicesOffset + 1]);
                uv.push_back(Eigen::Vector3f(1, 0, 0));
            }
            angle += angleDelta;
            lastBottom = bottom;
            lastS = s;
            lastNormal = normal;
        }

        if (bottomRadius != 0)
        {
            indicesOffset = verts.size();
            angle = 0;
            for (int i = 0; i < sec; ++i, angle += angleDelta)
            {
                float sinAngle = sin(angle);
                float cosAngle = cos(angle);

                if (i > 2)
                {
                    verts.push_back(verts[indicesOffset]);
                    normals.push_back(normals[indicesOffset]);
                    uv.push_back(uv[indicesOffset]);

                    verts.push_back(verts[verts.size() - 2]);
                    normals.push_back(normals[normals.size() - 2]);
                    uv.push_back(uv[uv.size() - 2]);
                }
                verts.push_back(Eigen::Vector3f(sinAngle * bottomRadius, start, cosAngle * bottomRadius));
                normals.push_back(Eigen::Vector3f(0, -1, 0));
                uv.push_back(Eigen::Vector3f(cosAngle * 0.5f + 0.5f, sinAngle * 0.5f + 0.5f, 0));

                if (i == sec - 1)
                {
                    verts.push_back(verts[indicesOffset]);
                    normals.push_back(normals[indicesOffset]);
                    uv.push_back(uv[indicesOffset]);

                    verts.push_back(verts[verts.size() - 2]);
                    normals.push_back(normals[normals.size() - 2]);
                    uv.push_back(uv[uv.size() - 2]);

                    verts.push_back(verts[indicesOffset]);
                    normals.push_back(normals[indicesOffset]);
                    uv.push_back(uv[indicesOffset]);
                }
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

                if (i > 2)
                {
                    verts.push_back(verts[indicesOffset]);
                    normals.push_back(normals[indicesOffset]);
                    uv.push_back(uv[indicesOffset]);

                    verts.push_back(verts[verts.size() - 2]);
                    normals.push_back(normals[normals.size() - 2]);
                    uv.push_back(uv[uv.size() - 2]);
                }

                verts.push_back(Eigen::Vector3f(sinAngle * upRadius, start + length, cosAngle * upRadius));
                normals.push_back(Eigen::Vector3f(0, 1, 0));
                uv.push_back(Eigen::Vector3f(cosAngle * 0.5f + 0.5f, sinAngle * 0.5f + 0.5f, 0));

                if (i == sec - 1)
                {
                    verts.push_back(verts[indicesOffset]);
                    normals.push_back(normals[indicesOffset]);
                    uv.push_back(uv[indicesOffset]);

                    verts.push_back(verts[verts.size() - 2]);
                    normals.push_back(normals[normals.size() - 2]);
                    uv.push_back(uv[uv.size() - 2]);

                    verts.push_back(verts[indicesOffset]);
                    normals.push_back(normals[indicesOffset]);
                    uv.push_back(uv[indicesOffset]);
                }
            }
        }
    }

    StaticMesh::SP GenArrowMesh(float thickness, float arrowThickness, float length, bool shareVertex)
    {
        std::vector<Eigen::Vector3f> verts;
        std::vector<Eigen::Vector3f> normals;
        std::vector<uint32_t> indices;
        std::vector<Eigen::Vector3f> uv0;

        auto ret = std::make_shared<StaticMesh>();

        if (shareVertex)
        {
            // body
            GenUpwardCylinder(verts, normals, indices, uv0, thickness, thickness, length, 0);
            // head
            GenUpwardCylinder(verts, normals, indices, uv0, arrowThickness, 0, arrowThickness * 2, length);

            ret->SetPositions(std::move(verts));
            ret->SetNormals(std::move(normals));
            ret->SetIndices(std::move(indices));
            ret->SetUvs(uv0, 0);
        }
        else
        {
            // body
            GenUpwardCylinder(verts, normals, uv0, thickness, thickness, length, 0);
            // head
            GenUpwardCylinder(verts, normals, uv0, arrowThickness, 0, arrowThickness * 2, length);
            ret->SetPositions(std::move(verts));
            ret->SetNormals(std::move(normals));
            ret->SetUvs(uv0, 0);
        }

        return ret;
    }

    StaticMesh::SP GenCubeMesh(Eigen::Vector3f size)
    {
        std::vector<Eigen::Vector3f> verts;
        std::vector<Eigen::Vector3f> normals;
        std::vector<Eigen::Vector3f> uv0;

        auto halfSz = size * 0.5;
        auto ret = std::make_shared<StaticMesh>();

        std::vector<int> indices = {
            1, 3, 5, 5, 3, 7,
            0, 4, 2, 2, 4, 6,
            2, 6, 3, 3, 6, 7,
            0, 1, 4, 4, 1, 5,
            4, 5, 6, 6, 5, 7,
            0, 2, 1, 1, 2, 3};
        Eigen::Vector3f uvs[6] = {
            {0, 0, 0},
            {1, 0, 0},
            {0, 1, 0},
            {0, 1, 0},
            {1, 0, 0},
            {1, 1, 0}};

        Eigen::Vector3f ns[6] = {
            {1, 0, 0},
            {-1, 0, 0},
            {0, 1, 0},
            {0, -1, 0},
            {0, 0, 1},
            {0, 0, -1}};
        // Centered at zero
        for (int i = 0; i < indices.size(); ++i)
        {
            int idx = indices[i];
            Eigen::Vector3f p = {
                halfSz.x() * (2 * ((idx & 1) ? 1 : 0) - 1), // x axis
                halfSz.y() * (2 * ((idx & 2) ? 1 : 0) - 1), // y axis
                halfSz.z() * (2 * ((idx & 4) ? 1 : 0) - 1)  // z axis
            };

            verts.push_back(p);
            uv0.push_back(uvs[i % 6]);
            normals.push_back(ns[i / 6]);
        }

        ret->SetPositions(std::move(verts));
        ret->SetNormals(std::move(normals));
        ret->SetUvs(uv0, 0);
        return ret;
    }

    StaticMesh::SP GenSphereMesh(float radius, int section)
    {
        std::vector<Eigen::Vector3f> verts;
        std::vector<Eigen::Vector3f> normals;
        std::vector<Eigen::Vector3f> uv0;

        int secPhi = section;
        int secTheta = secPhi * 2;
        float deltaPhi = (float)PI / secPhi;
        float deltaTheta = 2 * (float)PI / secTheta;

        float phi = (float)(-PI * 0.5);
        float theta = 0;

        std::vector<Eigen::Vector3f> prevRow;
        std::vector<Eigen::Vector3f> currentRow;
        std::vector<float> circleU;

        for (int thetaIdx = 0; thetaIdx < secTheta; ++thetaIdx)
        {
            circleU.push_back((float)thetaIdx / secTheta * 2);
        }
        circleU.push_back(2);

        float lastv = 0;
        theta = 0;
        for (int phiIdx = 0; phiIdx < secPhi + 1; ++phiIdx)
        {
            theta = 0;
            float v = (float)phiIdx / secPhi;
            if (phiIdx == secPhi)
                phi = (float)(PI * 0.5);
            float sinP = sinf(phi);
            float cosP = cosf(phi);
            for (int thetaIdx = 0; thetaIdx < secTheta; ++thetaIdx)
            {
                float cosT = cosf(theta);
                float sinT = sinf(theta);

                Eigen::Vector3f p = {radius * cosP * cosT, radius * sinP, radius * cosP * sinT};

                currentRow.push_back(p);
                theta += deltaTheta;
            }
            currentRow.push_back(currentRow[0]);

            if (!prevRow.empty())
            {
                for (int i = 0; i < secTheta; ++i)
                {
                    int idx = i;
                    int nextIdx = idx + 1;

                    verts.push_back(currentRow[idx]);
                    normals.push_back(currentRow[idx].normalized());
                    uv0.push_back(Eigen::Vector3f(circleU[idx], v, 0));

                    verts.push_back(currentRow[nextIdx]);
                    normals.push_back(currentRow[nextIdx].normalized());
                    uv0.push_back(Eigen::Vector3f(circleU[nextIdx], v, 0));

                    verts.push_back(prevRow[idx]);
                    normals.push_back(prevRow[idx].normalized());
                    uv0.push_back(Eigen::Vector3f(circleU[idx], lastv, 0));

                    verts.push_back(prevRow[idx]);
                    normals.push_back(prevRow[idx].normalized());
                    uv0.push_back(Eigen::Vector3f(circleU[idx], lastv, 0));

                    verts.push_back(currentRow[nextIdx]);
                    normals.push_back(currentRow[nextIdx].normalized());
                    uv0.push_back(Eigen::Vector3f(circleU[nextIdx], v, 0));

                    verts.push_back(prevRow[nextIdx]);
                    normals.push_back(prevRow[nextIdx].normalized());
                    uv0.push_back(Eigen::Vector3f(circleU[nextIdx], lastv, 0));
                }
            }

            prevRow.swap(currentRow);
            currentRow.clear();
            lastv = v;
            phi += deltaPhi;
        }

        auto ret = std::make_shared<StaticMesh>();
        ret->SetPositions(verts);
        ret->SetUvs(uv0, 0);
        ret->SetNormals(normals);

        return ret;
    }
}