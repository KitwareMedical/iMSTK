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

#ifndef RENDER_CUBE_OCULUS_H
#define RENDER_CUBE_OCULUS_H

#include "smCore/smSDK.h"
#include "smRendering/smOculusViewer.h"

class RenderCubeOculus : public smSimulationMain, public smCoreClass
{
public:
    RenderCubeOculus();
    void simulateMain(smSimulationMainParam p_param);
private:
    smSDK* simmedtkSDK;
    smScene *scene1;
    smStaticSceneObject cube;
    smOculusViewer viewer;

    void setupLights();
    void setupCamera();
};

void runRenderCubeOculus();

#endif
