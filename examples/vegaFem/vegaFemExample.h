/*=========================================================================
 * Copyright (c) Center for Modeling, Simulation, and Imaging in Medicine,
 *                        Rensselaer Polytechnic Institute
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 /=========================================================================
 
 /**
  *  \brief
  *  \details
  *  \author
  *  \author
  *  \copyright Apache License, Version 2.0.
  */

#include "smCore/smConfig.h"
#include "smCore/smErrorLog.h"
#include "smCore/smCoreClass.h"
#include "smCore/smSDK.h"
#include "smRendering/smViewer.h"
#include "smSimulators/smVegaFemSimulator.h"
#include "smSimulators/smVegaFemSceneObject.h"

class vegaFemExample : public smSimulationMain, public smCoreClass
{

public:
    vegaFemExample();
    void simulateMain(smSimulationMainParam p_param) { };
private:
    smSDK *sofmisSDK;
    smVegaFemSceneObject *femobj;
    smVegaFemSimulator *femSim;
    smMatrix33<smFloat> mat;
    smSimulator *simulator;
    smViewer *viewer;
    smScene *scene1;
};

void VegaFemExample();
