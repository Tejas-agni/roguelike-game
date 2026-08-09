#include "audio/AudioManager.h"
#include "core/Logger.h"
#include <algorithm>

namespace Roguelike {

AudioManager::AudioManager(ResourceManager& resourceManager)
    : m_ResourceManager(resourceManager)
{
    InitAudioDevice();
    if (IsAudioDeviceReady()) {
        Logger::Info("Audio device initialized successfully.");
    } else {
        Logger::Error("Failed to initialize audio device!");
    }
}

AudioManager::~AudioManager() {
    CloseAudioDevice();
    Logger::Info("Audio device closed.");
}

void AudioManager::Update() {
    if (m_CurrentMusic != nullptr) {
        UpdateMusicStream(*m_CurrentMusic);
    }
}

void AudioManager::PlaySFX(std::string_view soundId, float pitch) {
    const Sound* sound = m_ResourceManager.GetSound(soundId);
    if (sound != nullptr) {
        SetSoundVolume(*sound, m_SFXVolume * m_MasterVolume);
        SetSoundPitch(*sound, pitch);
        PlaySound(*sound);
    }
}

void AudioManager::PlayMusic(std::string_view musicId, bool loop) {
    Music* mus = m_ResourceManager.GetMusic(musicId);
    if (mus != nullptr) {
        if (m_CurrentMusic != nullptr) {
            StopMusicStream(*m_CurrentMusic);
        }
        m_CurrentMusic = mus;
        mus->looping = loop;
        ::SetMusicVolume(*m_CurrentMusic, m_MusicVolume * m_MasterVolume);
        PlayMusicStream(*m_CurrentMusic);
        Logger::Info("Playing music stream: {}", musicId);
    }
}

void AudioManager::StopMusic() {
    if (m_CurrentMusic != nullptr) {
        StopMusicStream(*m_CurrentMusic);
        m_CurrentMusic = nullptr;
    }
}

void AudioManager::SetMasterVolume(float volume) {
    m_MasterVolume = std::clamp(volume, 0.0f, 1.0f);
    ::SetMasterVolume(m_MasterVolume);
}

void AudioManager::SetSFXVolume(float volume) {
    m_SFXVolume = std::clamp(volume, 0.0f, 1.0f);
}

void AudioManager::SetMusicVolume(float volume) {
    m_MusicVolume = std::clamp(volume, 0.0f, 1.0f);
    if (m_CurrentMusic != nullptr) {
        ::SetMusicVolume(*m_CurrentMusic, m_MusicVolume * m_MasterVolume);
    }
}

} // namespace Roguelike
