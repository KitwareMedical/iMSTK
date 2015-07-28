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

#include <memory>

#include "Core/Geometry.h"
#include "Core/RenderDelegate.h"
#include "Core/Factory.h"
#include "Mesh/Mesh.h"

#include "VtkRendering/MeshNodalCoordinates.h"
#include "VtkRendering/VtkRenderDelegate.h"

// Vtk incudes
#include <vtkSmartPointer.h>
#include <vtkNew.h>
#include <vtkActor.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkGeometryFilter.h>
#include <vtkPolyDataNormals.h>


class MeshRenderDelegate : public VtkRenderDelegate
{
public:
    MeshRenderDelegate();
    virtual bool isTargetTextured() const override;

    vtkActor *getActor() const;
    void init();

private:
    vtkSmartPointer<vtkActor> actor;
    vtkSmartPointer<MeshNodalCoordinates<double>> mappedData;
    vtkSmartPointer<vtkMapper> mapper;
};

MeshRenderDelegate::MeshRenderDelegate()
{
    actor = vtkActor::New();
    mappedData = MeshNodalCoordinates<double>::New();

    vtkNew<vtkUnstructuredGrid> unstructuredMesh;
    vtkNew<vtkPoints> vertices;
    vtkNew<vtkCellArray> triangles;

    auto geom = this->getSourceGeometryAs<Mesh>();
    if (!geom)
    {
        return;
    }

    auto mesh = std::dynamic_pointer_cast<Mesh>(geom->shared_from_this());

    mappedData->SetVertexArray(mesh->getVertices());

    for(int i = 0; i < mesh->nbrTriangles; ++i)
    {
        vtkIdType    cell[3];
        cell[0] = mesh->triangles[i].vert[0];
        cell[1] = mesh->triangles[i].vert[1];
        cell[2] = mesh->triangles[i].vert[2];
        triangles->InsertNextCell(3,cell);
    }

    vertices->SetNumberOfPoints(mesh->getVertices().size());
    vertices->SetData(mappedData.Get());
    unstructuredMesh->SetPoints(vertices.GetPointer());
    unstructuredMesh->SetCells(VTK_TRIANGLE,triangles.GetPointer());

    if (geom->getRenderDetail()->renderType & SIMMEDTK_RENDER_NORMALS)
    {
        vtkSmartPointer<vtkGeometryFilter> geometry = vtkGeometryFilter::New();
        geometry->SetInputData(unstructuredMesh.GetPointer());

        vtkSmartPointer<vtkPolyDataNormals> normals = vtkPolyDataNormals::New();
        normals->SetInputConnection(geometry->GetOutputPort());

        mapper = vtkPolyDataMapper::New();
        mapper->SetInputConnection(normals->GetOutputPort());
    }
    else
    {
        mapper = vtkDataSetMapper::New();
        mapper->SetInputDataObject(unstructuredMesh.GetPointer());
    }

    actor->SetMapper(mapper);
}

bool MeshRenderDelegate::isTargetTextured() const
{
    auto geom = this->getSourceGeometryAs<Mesh>();
    if (!geom)
        return false;

    return geom->isMeshTextured();
}

vtkActor *MeshRenderDelegate::getActor() const
{
    return this->actor.GetPointer();
}

SIMMEDTK_BEGIN_DYNAMIC_LOADER()
SIMMEDTK_BEGIN_ONLOAD(register_mesh_render_delegate)
    SIMMEDTK_REGISTER_CLASS(RenderDelegate,RenderDelegate,MeshRenderDelegate,300);
SIMMEDTK_FINISH_ONLOAD()
SIMMEDTK_FINISH_DYNAMIC_LOADER()
