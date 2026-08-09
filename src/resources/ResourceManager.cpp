#include "resources/ResourceManager.h"
#include "core/Logger.h"
#include <vector>
#include <cstdlib>
#include <cmath>

namespace Roguelike {

ResourceManager::~ResourceManager() {
    UnloadAll();
}

bool ResourceManager::LoadTexture(std::string_view id, std::string_view filePath) {
    if (m_Textures.contains(std::string(id))) return true;

    Texture2D tex = ::LoadTexture(filePath.data());
    if (tex.id == 0) {
        Logger::Error("Failed to load texture: {}", filePath);
        return false;
    }

    SetTextureFilter(tex, TEXTURE_FILTER_POINT);
    m_Textures.emplace(std::string(id), tex);
    Logger::Info("Loaded texture: {} -> {}", id, filePath);
    return true;
}

const Texture2D* ResourceManager::GetTexture(std::string_view id) const {
    auto it = m_Textures.find(std::string(id));
    if (it != m_Textures.end()) return &it->second;
    Logger::Warn("Texture not found: {}", id);
    return nullptr;
}

bool ResourceManager::LoadSound(std::string_view id, std::string_view filePath) {
    if (m_Sounds.contains(std::string(id))) return true;

    Sound sfx = ::LoadSound(filePath.data());
    if (sfx.frameCount == 0) {
        Logger::Error("Failed to load sound: {}", filePath);
        return false;
    }

    m_Sounds.emplace(std::string(id), sfx);
    Logger::Info("Loaded sound: {} -> {}", id, filePath);
    return true;
}

bool ResourceManager::GenerateProceduralSound(std::string_view id, float baseFreq, float duration, int type) {
    if (m_Sounds.contains(std::string(id))) return true;

    const int sampleRate = 44100;
    const int totalSamples = static_cast<int>(sampleRate * duration);
    std::vector<short> pcmData(totalSamples);

    for (int i = 0; i < totalSamples; ++i) {
        float t = static_cast<float>(i) / sampleRate;
        float progress = static_cast<float>(i) / totalSamples;
        float envelope = 1.0f - progress; // Exponential decay

        float sample = 0.0f;
        if (type == 0) { // Square wave (Retro blip / hit)
            sample = (sinf(2.0f * PI * baseFreq * t) > 0.0f) ? 0.3f : -0.3f;
        } else if (type == 1) { // Sine wave (Chime / Gem pickup)
            sample = sinf(2.0f * PI * baseFreq * t) * 0.4f;
        } else if (type == 2) { // Noise wave (Explosion / Kill)
            sample = (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * 0.4f;
        }

        pcmData[i] = static_cast<short>(sample * envelope * 32767.0f);
    }

    Wave wave{};
    wave.frameCount = totalSamples;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = pcmData.data();

    Sound sound = ::LoadSoundFromWave(wave);
    m_Sounds.emplace(std::string(id), sound);
    Logger::Info("Synthesized procedural sound: {} (Freq: {:.0f}Hz, Duration: {:.2f}s)", id, baseFreq, duration);
    return true;
}

const Sound* ResourceManager::GetSound(std::string_view id) const {
    auto it = m_Sounds.find(std::string(id));
    if (it != m_Sounds.end()) return &it->second;
    Logger::Warn("Sound not found: {}", id);
    return nullptr;
}

bool ResourceManager::LoadMusic(std::string_view id, std::string_view filePath) {
    if (m_MusicStreams.contains(std::string(id))) return true;

    Music mus = ::LoadMusicStream(filePath.data());
    if (mus.ctxData == nullptr) {
        Logger::Error("Failed to load music stream: {}", filePath);
        return false;
    }

    m_MusicStreams.emplace(std::string(id), mus);
    Logger::Info("Loaded music stream: {} -> {}", id, filePath);
    return true;
}

Music* ResourceManager::GetMusic(std::string_view id) {
    auto it = m_MusicStreams.find(std::string(id));
    if (it != m_MusicStreams.end()) return &it->second;
    Logger::Warn("Music stream not found: {}", id);
    return nullptr;
}

bool ResourceManager::LoadFont(std::string_view id, std::string_view filePath, int fontSize) {
    if (m_Fonts.contains(std::string(id))) return true;

    Font font = ::LoadFontEx(filePath.data(), fontSize, nullptr, 0);
    if (font.texture.id == 0) {
        Logger::Error("Failed to load font: {}", filePath);
        return false;
    }

    SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
    m_Fonts.emplace(std::string(id), font);
    Logger::Info("Loaded font: {} -> {}", id, filePath);
    return true;
}

const Font* ResourceManager::GetFont(std::string_view id) const {
    auto it = m_Fonts.find(std::string(id));
    if (it != m_Fonts.end()) return &it->second;
    return nullptr;
}

void ResourceManager::UnloadAll() {
    for (auto& [id, tex] : m_Textures) ::UnloadTexture(tex);
    for (auto& [id, sfx] : m_Sounds) ::UnloadSound(sfx);
    for (auto& [id, mus] : m_MusicStreams) ::UnloadMusicStream(mus);
    for (auto& [id, font] : m_Fonts) ::UnloadFont(font);

    m_Textures.clear();
    m_Sounds.clear();
    m_MusicStreams.clear();
    m_Fonts.clear();
    Logger::Info("All resources unloaded.");
}

} // namespace Roguelike
