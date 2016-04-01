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

#include "imstkSimulationManager.h"

#include <string>

#include "g3log/g3log.hpp"

namespace imstk {
const
SimulationStatus& SimulationManager::getStatus() const
{
    return m_status;
}

bool
SimulationManager::isSceneRegistered(std::string sceneName) const
{
    return m_sceneMap.find(sceneName) != m_sceneMap.end();
}

std::shared_ptr<Scene>
SimulationManager::getScene(std::string sceneName) const
{
    if (!this->isSceneRegistered(sceneName))
    {
        LOG(WARNING) << "No scene named '" << sceneName
                     << "' was registered in this simulation.";
        return nullptr;
    }

    return m_sceneMap.at(sceneName);
}

std::shared_ptr<Scene>
SimulationManager::createNewScene(std::string newSceneName)
{
    if (this->isSceneRegistered(newSceneName))
    {
        LOG(WARNING) << "Can not create new scene: '" << newSceneName
                     << "' is already registered in this simulation.\n"
                     << "You can create a new scene using an unique name.";
        return nullptr;
    }

    m_sceneMap[newSceneName] = std::make_shared<Scene>(newSceneName);
    LOG(INFO) << "New scene added: " << newSceneName;
    return m_sceneMap.at(newSceneName);
}

std::shared_ptr<Scene>
SimulationManager::createNewScene()
{
    int id                   = m_sceneMap.size() + 1;
    std::string newSceneName = "Scene_" + std::to_string(id);

    return this->createNewScene(newSceneName);
}

void
SimulationManager::addScene(std::shared_ptr<Scene>newScene)
{
    std::string newSceneName = newScene->getName();

    if (this->isSceneRegistered(newSceneName))
    {
        LOG(WARNING) << "Can not add scene: '" << newSceneName
                     << "' is already registered in this simulation.\n"
                     << "Set this scene name to a unique name first.";
        return;
    }

    m_sceneMap[newSceneName] = newScene;
    LOG(INFO) << "Scene added: " << newSceneName;
}

void
SimulationManager::removeScene(std::string sceneName)
{
    if (!this->isSceneRegistered(sceneName))
    {
        LOG(WARNING) << "No scene named '" << sceneName
                     << "' was registered in this simulation.";
        return;
    }

    m_sceneMap.erase(sceneName);
    LOG(INFO) << "Scene removed: " << sceneName;
}

std::shared_ptr<Viewer>
SimulationManager::getViewer() const
{
    return m_viewer;
}

void
SimulationManager::startSimulation(std::string sceneName)
{
    LOG(INFO) << "Starting simulation.";

    if (m_status != SimulationStatus::INACTIVE)
    {
        LOG(WARNING) << "Simulation already active.";
        return;
    }

    std::shared_ptr<Scene> startingScene = this->getScene(sceneName);

    if (!startingScene)
    {
        LOG(WARNING) << "Simulation canceled.";
        return;
    }

    if (startingScene->getStatus() != ModuleStatus::INACTIVE)
    {
        LOG(WARNING) << "Scene '" << sceneName << "' is already active.\n"
                     << "Simulation canceled.";
        return;
    }

    // Init viewer renderer
    m_viewer->setCurrentScene(startingScene);
    m_viewer->initRenderer();

    // Start scene
    this->startModuleInNewThread(startingScene);
    m_currentSceneName = sceneName;

    // Update simulation status
    m_status = SimulationStatus::RUNNING;

    // Start Rendering
    m_viewer->startRenderingLoop();
    this->endSimulation();
}

void
SimulationManager::switchScene(std::string newSceneName, bool unloadCurrentScene)
{
    LOG(INFO) << "Switching scene.";

    if ((m_status != SimulationStatus::RUNNING) &&
        (m_status != SimulationStatus::PAUSED))
    {
        LOG(WARNING) << "Simulation not active, can not switch scenes.";
        return;
    }

    if (newSceneName == m_currentSceneName)
    {
        LOG(WARNING) << "Scene '" << newSceneName << "' is already running.";
        return;
    }

    std::shared_ptr<Scene> newScene = this->getScene(newSceneName);

    if (!newScene)
    {
        LOG(WARNING) << "Can not switch scenes.";
        return;
    }

    // TODO : update viewer

    if (unloadCurrentScene)
    {
        // Stop current scene
        LOG(INFO) << "Unloading '" << m_currentSceneName << "'.";
        m_sceneMap.at(m_currentSceneName)->end();
        m_threadMap.at(m_currentSceneName).join();
    }
    else
    {
        // Pause current scene
        m_sceneMap.at(m_currentSceneName)->pause();
    }

    if (newScene->getStatus() == ModuleStatus::INACTIVE)
    {
        // Start new scene
        this->startModuleInNewThread(newScene);
    }
    else if (newScene->getStatus() == ModuleStatus::PAUSED)
    {
        // Run new scene
        newScene->run();
    }
    m_currentSceneName = newSceneName;
}

void
SimulationManager::runSimulation()
{
    LOG(INFO) << "Running simulation.";

    if (m_status != SimulationStatus::PAUSED)
    {
        LOG(WARNING) << "Simulation not paused, can not run.";
        return;
    }

    // Run scene
    m_sceneMap.at(m_currentSceneName)->run();

    // Update simulation status
    m_status = SimulationStatus::RUNNING;
}

void
SimulationManager::pauseSimulation()
{
    LOG(INFO) << "Pausing simulation.";

    if (m_status != SimulationStatus::RUNNING)
    {
        LOG(WARNING) << "Simulation not running, can not pause.";
        return;
    }

    // Pause scene
    m_sceneMap.at(m_currentSceneName)->pause();

    // Update simulation status
    m_status = SimulationStatus::PAUSED;
}

void
SimulationManager::endSimulation()
{
    LOG(INFO) << "Ending simulation.";

    if ((m_status != SimulationStatus::RUNNING) &&
        (m_status != SimulationStatus::PAUSED))
    {
        LOG(WARNING) << "Simulation already terminated.";
        return;
    }

    // End all scenes
    for (auto pair : m_sceneMap)
    {
        std::string  sceneName   = pair.first;
        ModuleStatus sceneStatus = m_sceneMap.at(sceneName)->getStatus();

        if (sceneStatus != ModuleStatus::INACTIVE)
        {
            m_sceneMap.at(sceneName)->end();
            m_threadMap.at(sceneName).join();
        }
    }

    // Update simulation status
    m_status = SimulationStatus::INACTIVE;
}

void
SimulationManager::startModuleInNewThread(std::shared_ptr<Module>module)
{
    m_threadMap[module->getName()] = std::thread([module] { module->start(); });
}
}
