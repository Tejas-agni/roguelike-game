#pragma once
#include "utils/NonCopyable.h"
#include "scene/Scene.h"
#include <memory>
#include <vector>

namespace Roguelike {

class SceneManager : public NonCopyable {
public:
    SceneManager() = default;
    ~SceneManager();

    void PushScene(std::unique_ptr<Scene> scene);
    void PopScene();
    void ChangeScene(std::unique_ptr<Scene> scene);

    void ProcessPendingChanges();

    void FixedUpdate(float fixedDeltaTime);
    void Update(float deltaTime);
    void RenderPixelPass();
    void RenderUIPass();

    [[nodiscard]] bool IsEmpty() const noexcept { return m_Scenes.empty(); }

private:
    enum class ActionType { Push, Pop, Change };
    struct PendingAction {
        ActionType type;
        std::unique_ptr<Scene> scene;
    };

    std::vector<std::unique_ptr<Scene>> m_Scenes;
    std::vector<PendingAction> m_PendingActions;
};

} // namespace Roguelike
