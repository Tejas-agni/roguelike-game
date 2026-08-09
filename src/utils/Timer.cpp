#include "utils/Timer.h"
#include <algorithm>

namespace Roguelike {

Timer::Timer(float durationSeconds, bool autoLoop)
    : m_Duration(durationSeconds)
    , m_AutoLoop(autoLoop)
{
}

void Timer::Start(float durationSeconds) {
    m_Duration = durationSeconds;
    Start();
}

void Timer::Start() {
    m_Elapsed = 0.0f;
    m_Running = true;
    m_Finished = false;
}

void Timer::Stop() {
    m_Running = false;
}

void Timer::Reset() {
    m_Elapsed = 0.0f;
    m_Finished = false;
}

void Timer::Update(float deltaTime) {
    if (!m_Running) return;

    m_Elapsed += deltaTime;
    if (m_Elapsed >= m_Duration) {
        m_Finished = true;
        if (m_AutoLoop) {
            m_Elapsed -= m_Duration;
        } else {
            m_Running = false;
        }
    }
}

float Timer::GetProgress() const noexcept {
    if (m_Duration <= 0.0f) return 1.0f;
    return std::clamp(m_Elapsed / m_Duration, 0.0f, 1.0f);
}

} // namespace Roguelike
