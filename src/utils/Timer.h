#pragma once

namespace Roguelike {

class Timer {
public:
    explicit Timer(float durationSeconds = 0.0f, bool autoLoop = false);

    void Start(float durationSeconds);
    void Start();
    void Stop();
    void Reset();
    void Update(float deltaTime);

    [[nodiscard]] bool IsFinished() const noexcept { return m_Finished; }
    [[nodiscard]] bool IsRunning() const noexcept { return m_Running; }
    [[nodiscard]] float GetElapsed() const noexcept { return m_Elapsed; }
    [[nodiscard]] float GetDuration() const noexcept { return m_Duration; }
    [[nodiscard]] float GetProgress() const noexcept; // Returns 0.0f to 1.0f

    void SetAutoLoop(bool loop) noexcept { m_AutoLoop = loop; }
    void SetDuration(float duration) noexcept { m_Duration = duration; }

private:
    float m_Duration{0.0f};
    float m_Elapsed{0.0f};
    bool m_Running{false};
    bool m_Finished{false};
    bool m_AutoLoop{false};
};

} // namespace Roguelike
