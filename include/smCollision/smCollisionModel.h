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

#ifndef SM_COLLISIONMODEL_H
#define SM_COLLISIONMODEL_H

// SimMedTK includes
#include "smCore/smCoreClass.h"
#include "smCollision/smCollisionModelIterator.h"
#include "smUtilities/smMatrix44.h"

/// \brief !!
template<typename T> 
class smCollisionModel: public smCoreClass
{
protected:
	typedef smMatrix44<double> MatrixType;
	
public:
    /// \brief !!
    virtual void initStructure() = 0;

    /// \brief !!
    virtual void reCreateStructure() = 0;

    /// \brief !!
    virtual void updateStructure() = 0;

    /// \brief !!
    virtual void translateRot() = 0;

    /// \brief !!
    virtual void setTranslateRot(MatrixType &) = 0;

    /// \brief !!
    virtual smCollisionModelIterator<T> getLevelIterator(int level) = 0;

    /// \brief !!
    virtual smCollisionModelIterator<T> getLevelIterator() = 0;
};


#endif
