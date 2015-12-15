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

#ifndef SMGEOMETRY_H
#define SMGEOMETRY_H

// SimMedTK includes
#include "Core/Config.h"
#include "Core/Vector.h"
#include "Core/Matrix.h"

#include "Core/Factory.h"
#include "RenderDelegate.h"

class VisualArtifact
{
public:
  virtual void setRenderDelegate(RenderDelegate::Ptr delegate)
    {
    this->renderDelegate = delegate;
    if (delegate)
      this->renderDelegate->setSourceGeometry(this);
    }
  virtual void draw() const
    {
    if (this->renderDelegate)
      this->renderDelegate->draw();
    }

    /// \brief Get render delegate
    std::shared_ptr<RenderDelegate> getRenderDelegate() const
    {
        return this->renderDelegate;
    }
  RenderDelegate::Ptr renderDelegate;
};

class GeometryBase : public CoreClass
{
public:
    GeometryBase(){}
    ~GeometryBase(){}

    virtual void translate(const core::Vec3d &t) = 0;
    virtual void rotate(const core::Matrix33d &rot) = 0;
};

#endif
