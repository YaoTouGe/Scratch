#pragma once

#include <vector>
#include <Eigen/Core>
#include "StaticMesh.h"

namespace Graphics
{
    // Get arrow along y axis
    StaticMesh::SP GenArrowMesh(float thickness, float arrowThickness, float length, bool shareVertex = true);
}