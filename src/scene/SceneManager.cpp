#include "scene/SceneManager.h"
#include "core/Logger.h"

namespace Roguelike {

SceneManager::~SceneManager() {
    while (!m_Scenes.empty()) {
        m_Scenes.back()->OnExit();
        m_Scenes.pop_back();
    }
}

void SceneManager::PushScene(std::unique_ptr<Scene> scene) {
    m_PendingActions.push_back({ ActionType::Push, std::move(scene) });
}

void SceneManager::PopScene() {
    m_PendingActions.push_back({ ActionType::Pop, nullptr });
}

void SceneManager::ChangeScene(std::unique_ptr<Scene> scene) {
    m_PendingActions.push_back({ ActionType::Change, std::move(scene) });
}

void SceneManager::ProcessPendingChanges() {
    for (auto& action : m_PendingActions) {
        switch (action.type) {
            case ActionType::Push:
                if (action.scene != nullptr) {
                    action.scene->OnEnter();
                    m_Scenes.push_back(std::move(action.scene));
                    Logger::Info("Scene pushed. Stack depth: {}", m_Scenes.size());
                }
                break;

            case ActionType::Pop:
                if (!m_Scenes.empty()) {
                    m_Scenes.back()->OnExit();
                    m_Scenes.pop_back();
                    Logger::Info("Scene popped. Stack depth: {}", m_Scenes.size());
                }
                break;

            case ActionType::Change:
                while (!m_Scenes.empty()) {
                    m_Scenes.back()->OnExit();
                    m_Scenes.pop_back();
                }
                if (action.scene != nullptr) {
                    action.scene->OnEnter();
                    m_Scenes.push_back(std::move(action.scene));
                    Logger::Info("Scene changed.");
                }
                break;
        }
    }
    m_PendingActions.clear();
}

void SceneManager::FixedUpdate(float fixedDeltaTime) {
    if (!m_Scenes.empty()) {
        m_Scenes.back()->FixedUpdate(fixedDeltaTime);
    }
}

void SceneManager::Update(float deltaTime) {
    if (!m_Scenes.empty()) {
        m_Scenes.back()->Update(deltaTime);
    }
}

void SceneManager::RenderPixelPass() {
    if (!m_Scenes.empty()) {
        m_Scenes.back()->RenderPixelPass();
    }
}

void SceneManager::RenderUIPass() {
    if (!m_Scenes.empty()) {
        m_Scenes.back()->RenderUIPass();
    }
}

} // namespace Roguelike
