/*=========================================================================

Library: iMSTK

Copyright (c) Kitware, Inc. & Center for Modeling, Simulation,
& Imaging in Medicine, Rensselaer Polytechnic Institute.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0.txt

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=========================================================================*/

#include "imstkPointPicker.h"
#include "imstkCollisionUtils.h"
#include "imstkLineMesh.h"
#include "imstkOrientedBox.h"
#include "imstkPlane.h"
#include "imstkSphere.h"
#include "imstkSurfaceMesh.h"
#include "imstkTetrahedralMesh.h"
#include "imstkVecDataArray.h"

namespace imstk
{
void
PointPicker::requestUpdate()
{
    std::shared_ptr<Geometry> geomToPick = getInput(0);
    geomToPick->updatePostTransformData();
    if (auto surfMeshToPick = std::dynamic_pointer_cast<SurfaceMesh>(geomToPick))
    {
        std::shared_ptr<VecDataArray<double, 3>> verticesPtr = surfMeshToPick->getVertexPositions();
        const VecDataArray<double, 3>&           vertices    = *verticesPtr;
        std::shared_ptr<VecDataArray<int, 3>>    indicesPtr  = surfMeshToPick->getTriangleIndices();
        const VecDataArray<int, 3>&              indices     = *indicesPtr;

        // Brute force
        // For every cell
        double minSqrDist   = IMSTK_DOUBLE_MAX;
        int    minCellIndex = -1;
        Vec3d  minPt = Vec3d::Zero();
        for (int i = 0; i < indices.size(); i++)
        {
            const Vec3i& cell = indices[i];
            const Vec3d& a    = vertices[cell[0]];
            const Vec3d& b    = vertices[cell[1]];
            const Vec3d& c    = vertices[cell[2]];
            const Vec3d  n    = (b - a).cross(c - a).normalized();
            Vec3d        iPt  = Vec3d::Zero();
            if (CollisionUtils::testRayToPlane(m_rayStart, m_rayDir, vertices[cell[0]], n, iPt))
            {
                const Vec3d uvw = baryCentric(iPt, a, b, c);
                if (uvw[0] >= 0.0 && uvw[1] >= 0.0 && uvw[2] >= 0.0) // Check if within triangle
                {
                    const double sqrDist = (m_rayStart - iPt).squaredNorm();
                    if (sqrDist < minSqrDist)
                    {
                        minCellIndex = i;
                        minSqrDist   = sqrDist;
                        minPt = iPt;
                    }
                }
            }
        }

        if (minCellIndex != -1)
        {
            if (m_maxDist == -1.0 || minSqrDist < m_maxDist * m_maxDist)
            {
                m_results.push_back({ { minCellIndex }, 1, IMSTK_TRIANGLE, minPt });
            }
        }
    }
    else if (auto tetMeshToPick = std::dynamic_pointer_cast<TetrahedralMesh>(geomToPick))
    {
        // Current implementation just based off the triangle faces
        static int faces[4][3] = { { 0, 1, 2 }, { 1, 2, 3 }, { 0, 2, 3 }, { 0, 1, 3 } };

        std::shared_ptr<VecDataArray<double, 3>> verticesPtr = tetMeshToPick->getVertexPositions();
        const VecDataArray<double, 3>&           vertices    = *verticesPtr;
        std::shared_ptr<VecDataArray<int, 4>>    indicesPtr  = tetMeshToPick->getTetrahedraIndices();
        const VecDataArray<int, 4>&              indices     = *indicesPtr;

        // For every tet
        double minSqrDist   = IMSTK_DOUBLE_MAX;
        int    minCellIndex = -1;
        Vec3d  minPt = Vec3d::Zero();
        for (int i = 0; i < indices.size(); i++)
        {
            const Vec4i& tet = indices[i];

            // For every face
            for (int j = 0; j < 4; j++)
            {
                // Find intersection point and add constraints
                const Vec3d& a = vertices[tet[faces[j][0]]];
                const Vec3d& b = vertices[tet[faces[j][1]]];
                const Vec3d& c = vertices[tet[faces[j][2]]];

                Vec3d iPt;
                if (CollisionUtils::testRayToPlane(m_rayStart, m_rayDir, a,
                    (b - a).cross(c - a).normalized(), iPt))
                {
                    const Vec3d uvw = baryCentric(iPt, a, b, c);
                    if (uvw[0] >= 0.0 && uvw[1] >= 0.0 && uvw[2] >= 0.0) // Check if within triangle
                    {
                        // If within line bounds
                        const double sqrDist = (m_rayStart - iPt).squaredNorm();
                        if (sqrDist < minSqrDist)
                        {
                            minCellIndex = i;
                            minSqrDist   = sqrDist;
                            minPt = iPt;
                        }
                    }
                }
            }
        }

        if (minCellIndex != -1)
        {
            if (m_maxDist == -1.0 || minSqrDist < m_maxDist * m_maxDist)
            {
                m_results.push_back({ { minCellIndex }, 1, IMSTK_TETRAHEDRON, minPt });
            }
        }
    }
    else if (auto lineMeshToPick = std::dynamic_pointer_cast<LineMesh>(geomToPick))
    {
        // Requires a thickness
        LOG(FATAL) << "LineMesh picking not implemented yet";
    }
    else if (auto sphereToPick = std::dynamic_pointer_cast<Sphere>(geomToPick))
    {
        Vec3d iPt = Vec3d::Zero();
        if (CollisionUtils::testRayToSphere(m_rayStart, m_rayDir,
            sphereToPick->getPosition(), sphereToPick->getRadius(), iPt))
        {
            const double minSqrDist = (m_rayStart - iPt).squaredNorm();
            if (m_maxDist == -1.0 || minSqrDist < m_maxDist * m_maxDist)
            {
                m_results.push_back({ { 0 }, 1, IMSTK_VERTEX, iPt });
            }
        }
    }
    else if (auto planeToPick = std::dynamic_pointer_cast<Plane>(geomToPick))
    {
        Vec3d iPt = Vec3d::Zero();
        if (CollisionUtils::testRayToPlane(m_rayStart, m_rayDir,
            planeToPick->getPosition(), planeToPick->getNormal(), iPt))
        {
            const double minSqrDist = (m_rayStart - iPt).squaredNorm();
            if (m_maxDist == -1.0 || minSqrDist < m_maxDist * m_maxDist)
            {
                m_results.push_back({ { 0 }, 1, IMSTK_VERTEX, iPt });
            }
        }
    }
    //else if (auto capsuleToPick = std::dynamic_pointer_cast<Capsule>(geomToPick))
    //{
    //}
    else if (auto obbToPick = std::dynamic_pointer_cast<OrientedBox>(geomToPick))
    {
        Mat4d worldToBox = mat4dTranslate(obbToPick->getPosition()) *
                           mat4dRotation(obbToPick->getOrientation());
        Vec2d t = Vec2d::Zero(); // Entry and exit t
        if (CollisionUtils::testRayToObb(m_rayStart, m_rayDir,
            worldToBox.inverse(), obbToPick->getExtents(), t))
        {
            const Vec3d  iPt = m_rayStart + m_rayDir * t[0];
            const double minSqrDist = (m_rayStart - iPt).squaredNorm();
            if (m_maxDist == -1.0 || minSqrDist < m_maxDist * m_maxDist)
            {
                m_results.push_back({ { 0 }, 1, IMSTK_VERTEX, iPt });
            }
        }
    }
    else if (auto implicitGeom = std::dynamic_pointer_cast<ImplicitGeometry>(geomToPick))
    {
        // Implicit primitives such as capsule should get caught here if the
        // above analytic solution is not provided. SDFs as well
        // Only works with bounded geometries

        // Find the intersection point on the oriented box
        Vec3d min, max;
        implicitGeom->computeBoundingBox(min, max);
        const Vec3d center  = (min + max) * 0.5;
        const Vec3d extents = (max - min) * 0.5; // Half the size

        const Mat4d boxTransform = mat4dTranslate(center);
        Vec2d       t = Vec2d::Zero(); // Entry and exit t
        if (CollisionUtils::testRayToObb(m_rayStart, m_rayDir, boxTransform.inverse(), extents, t))
        {
            // If it hit, start iterating from this point on the box in the implicit geometry
            Vec3d iPt = m_rayStart + m_rayDir * t[0];

            // For implicit geometry this isn't always signed distance
            double currDist = IMSTK_DOUBLE_MAX;
            Vec3d  currPt   = iPt;
            double prevDist = IMSTK_DOUBLE_MAX;
            Vec3d  prevPt   = iPt;
            for (int i = 0; i < 50; i++)
            {
                // Push back state
                prevDist = currDist;
                prevPt   = currPt;

                // Compute new pt
                const double t = static_cast<double>(i) / 50.0;
                currPt   = iPt + t * m_rayDir * 50.0;
                currDist = implicitGeom->getFunctionValue(currPt);

                // If it turned negative
                if (currDist < 0.0)
                {
                    // Use midpoint of results
                    iPt = (currPt + prevPt) * 0.5;
                    const double minSqrDist = (m_rayStart - iPt).squaredNorm();
                    if (m_maxDist == -1.0 || minSqrDist < m_maxDist * m_maxDist)
                    {
                        m_results.push_back({ { 0 }, 1, IMSTK_VERTEX, iPt });
                    }
                }
            }
        }
    }
    else
    {
        LOG(FATAL) << "Tried to point pick with an unsupported geometry " << geomToPick->getTypeName();
    }
}
} // namespace imstk