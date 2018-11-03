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

#include "imstkVTKHexahedralMeshRenderDelegate.h"

#include "imstkHexahedralMesh.h"

#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkPoints.h>
#include <vtkDoubleArray.h>
#include <vtkCellArray.h>
#include <vtkProperty.h>

namespace imstk
{
VTKHexahedralMeshRenderDelegate::VTKHexahedralMeshRenderDelegate(std::shared_ptr<HexahedralMesh> hexahedralMesh) :
    m_geometry(hexahedralMesh),
    m_mappedVertexArray(vtkSmartPointer<vtkDoubleArray>::New())
{
    // Map vertices
    StdVectorOfVec3d& vertices = m_geometry->getVertexPositionsNotConst();
    double* vertData = reinterpret_cast<double*>(vertices.data());
    m_mappedVertexArray->SetNumberOfComponents(3);
    m_mappedVertexArray->SetArray(vertData, vertices.size()*3, 1);

    // Create points
    auto points = vtkSmartPointer<vtkPoints>::New();
    points->SetNumberOfPoints(m_geometry->getNumVertices());
    points->SetData(m_mappedVertexArray);

    // Copy cells
    auto cells = vtkSmartPointer<vtkCellArray>::New();
    vtkIdType cell[8];
    for(const auto &t : m_geometry->getHexahedraVertices())
    {
        for(size_t i = 0; i < 8; ++i)
        {
            cell[i] = t[i];
        }
        cells->InsertNextCell(8,cell);
    }

    // Create Unstructured Grid
    auto unstructuredGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
    unstructuredGrid->SetPoints(points);
    unstructuredGrid->SetCells(VTK_HEXAHEDRON, cells);
    m_geometry->m_dataModified = false;

    // Mapper & Actor
    auto mapper = vtkSmartPointer<vtkDataSetMapper>::New();
    mapper->SetInputData(unstructuredGrid);
    m_actor->SetMapper(mapper);

    // Update Transform, Render Properties
    this->update();
}

void
VTKHexahedralMeshRenderDelegate::updateDataSource()
{
    if (!m_geometry->m_dataModified)
    {
        return;
    }

    m_mappedVertexArray->Modified();

    m_geometry->m_dataModified = false;
}


std::shared_ptr<Geometry>
VTKHexahedralMeshRenderDelegate::getGeometry() const
{
    return m_geometry;
}

std::shared_ptr<imstk::RenderMaterial> 
VTKHexahedralMeshRenderDelegate::getRenderMaterial() const
{
    return m_geometry->getRenderMaterial();
}

} // imstk
