#pragma once
#include "utils/NonCopyable.h"
#include <raylib.h>
#include <string>
#include <string_view>
#include <unordered_map>

namespace Roguelike {

class ResourceManager : public NonCopyable {
public:
    ResourceManager() = default;
    ~ResourceManager();

    // Textures
    bool LoadTexture(std::string_view id, std::string_view filePath);
    [[nodiscard]] const Texture2D* GetTexture(std::string_view id) const;

    // Sound SFX
    bool LoadSound(std::string_view id, std::string_view filePath);
    bool GenerateProceduralSound(std::string_view id, float baseFreq, float duration, int type);
    [[nodiscard]] const Sound* GetSound(std::string_view id) const;

    // Music
    bool LoadMusic(std::string_view id, std::string_view filePath);
    [[nodiscard]] Music* GetMusic(std::string_view id);

    // Fonts
    bool LoadFont(std::string_view id, std::string_view filePath, int fontSize = 32);
    [[nodiscard]] const Font* GetFont(std::string_view id) const;

    void UnloadAll();

private:
    std::unordered_map<std::string, Texture2D> m_Textures;
    std::unordered_map<std::string, Sound> m_Sounds;
    std::unordered_map<std::string, Music> m_MusicStreams;
    std::unordered_map<std::string, Font> m_Fonts;
};

} // namespace Roguelike
