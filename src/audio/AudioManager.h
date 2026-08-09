#pragma once
#include "utils/NonCopyable.h"
#include "resources/ResourceManager.h"
#include <raylib.h>
#include <string_view>

namespace Roguelike {

class AudioManager : public NonCopyable {
public:
    explicit AudioManager(ResourceManager& resourceManager);
    ~AudioManager();

    void Update(); // Call once per frame to update active music streams

    void PlaySFX(std::string_view soundId, float pitch = 1.0f);
    void PlayMusic(std::string_view musicId, bool loop = true);
    void StopMusic();

    void SetMasterVolume(float volume);
    void SetSFXVolume(float volume);
    void SetMusicVolume(float volume);

    [[nodiscard]] ResourceManager& GetResourceManager() noexcept { return m_ResourceManager; }

    [[nodiscard]] float GetMasterVolume() const noexcept { return m_MasterVolume; }
    [[nodiscard]] float GetSFXVolume() const noexcept { return m_SFXVolume; }
    [[nodiscard]] float GetMusicVolume() const noexcept { return m_MusicVolume; }

private:
    ResourceManager& m_ResourceManager;
    Music* m_CurrentMusic{nullptr};
    float m_MasterVolume{1.0f};
    float m_SFXVolume{1.0f};
    float m_MusicVolume{1.0f};
};

} // namespace Roguelike
