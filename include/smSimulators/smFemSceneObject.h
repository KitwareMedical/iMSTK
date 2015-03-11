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

#ifndef SMFEMSCENEOBJECT_H
#define SMFEMSCENEOBJECT_H

#include "smCore/smConfig.h"
#include "smMesh/smVolumeMesh.h"
#include "smMesh/smSurfaceMesh.h"
#include "smCore/smCoreClass.h"
#include "smCore/smSceneObject.h"
#include "smRendering/smConfigRendering.h"
#include "smRendering/smCustomRenderer.h"

#include <Eigen/Core>
#include <Eigen/LU>
using namespace Eigen;


/// \brief Fem class
class smFemSceneObject: public smSceneObject
{
public:
    //fem objetc has two representations: surface and volume
    smVolumeMesh *v_mesh; ///< volume mesh
    MatrixXf LM; ///< LM matrix of FEM
    MatrixXf ID; ///< ID matrix of FEM
    smInt totalDof; ///< total degree of freedom of the fem mesh
    smFloat dT; ///< time step
    smFloat density; ///< density of material
    smBool dynamicFem; ///< true if the mesh is to be evolved in time
    smBool pullUp; ///< !!
    MatrixXf stiffnessMatrix; ///< FEM stiffness matrix
    MatrixXf Kinv; ///< inverse of FEM stiffness matrix
    Matrix3f k33; ///< holds a 3x3 matrix used during FEM formulation
    Matrix3f k33inv; ///< inverse of 3x3 matrix
    VectorXf displacements; ///< displacement vector of the FEM
    VectorXf temp_displacements; ///< temporary vector of displacements
    VectorXf displacements_prev; ///< displacement vector at previous time step
    VectorXf dymamic_temp; ///< !!
    VectorXf componentMasses; ///< !!
    VectorXf fm; ///< !!
    VectorXf fm_temp; ///< !!
    VectorXf totalDisp; ///< total displacement till a given time t
    VectorXf nodeMass; ///< nodal mass
    VectorXf viscosity; ///< viscosity
    smInt pulledNode; ///< ID of the node pulled by the user

    /// \brief constructor
    smFemSceneObject(smErrorLog *p_log = NULL)
    {
        type = SIMMEDTK_SMFEMSCENEOBJECT;
        v_mesh = new smVolumeMesh(SMMESH_DEFORMABLE, p_log);
        pullUp = true;
        dynamicFem = false;

        if (dynamicFem)
        {
            dT = 0.02;
            density = 500;
        }
    }

    /// \brief !! This function does not clone!, it simply returns a pointer...this is dangerous
    virtual smSceneObject* clone()
    {
        return this;
    }

    /// \brief build FEM LM matrix
    void buildLMmatrix();

    /// \brief compute FEM stiffness matrix
    void computeStiffness();

    /// \brief load a matrix from an external file
    smBool loadMatrix(const smChar *fname, MatrixXf &a);

    /// \brief save a matrix to an external file
    smBool saveMatrix(const smChar *fname, MatrixXf &a);

    /// \brief !!
    smFloat V(smInt xyz, smInt xyz123, smInt tet);

    /// \brief assemble FEM stiffness matrix
    void assembleK(smInt element, MatrixXf k);

    /// \brief compute the displacements for quasi-static simulation
    void calculateDisplacements_QStatic(smVec3f*);

    /// \brief compute the displacements for dynamic simulation
    void calculateDisplacements_Dynamic(smVec3f *vertices);

    /// \brief !!
    void lumpMasses();

    /// \brief get the volume of tetrahedron given four sides
    smFloat tetraVolume(Vector3f &a, Vector3f &b, Vector3f &c, Vector3f &d);

    /// \brief !!
    virtual void serialize(void *p_memoryBlock)
    {
    }
    /// \brief !!
    virtual void unSerialize(void *p_memoryBlock)
    {

    }

    /// \brief render the FEM mesh
    virtual void draw(smDrawParam p_params);
};

#endif
