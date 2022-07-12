#pragma once

#include <vector>
#include <Eigen/Core>
#include "StaticMesh.h"

namespace Graphics
{
    // Get arrow along y axis
    StaticMesh::SP GenArrowMesh(float thickness, float arrowThickness, float length, bool shareVertex = true);
    // Cube centered at origin
    StaticMesh::SP GenCubeMesh(Eigen::Vector3f size);
    StaticMesh::SP GenSphereMesh(float radius);
    StaticMesh::SP GenPlaneMesh();
    StaticMesh::SP GenTubeMesh();
}