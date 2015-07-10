// This file is part of the SimMedTK project.
// Copyright (c) Center for Modeling, Simulation, and Imaging in Medicine,
//                        Rensselaer Polytechnic Institute
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//---------------------------------------------------------------------------
//
// Authors:
//
// Contact:
//---------------------------------------------------------------------------

#ifndef SM_COLLISIONUTILITIES_H
#define SM_COLLISIONUTILITIES_H

// SimMedTK includes
#include "smCore/smVector.h"

/// \brief If the point is in the triangle, it will also send the barycentric coord.
int intersect3D_RayTriangleWithBarycentric(smVec3d P0, smVec3d P1, smVec3d V0,
        smVec3d V1, smVec3d V2, smVec3d *I,
        float &p_baryU, float &p_baryV,
        float &p_baryW,
        bool considerFrontFaceOnly);


#endif // SM_COLLISIONUTILITIES_H