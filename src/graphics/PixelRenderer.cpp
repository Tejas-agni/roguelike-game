#include "graphics/PixelRenderer.h"
#include "core/Logger.h"

namespace Roguelike {

PixelRenderer::PixelRenderer(int renderWidth, int renderHeight)
    : m_RenderWidth(renderWidth)
    , m_RenderHeight(renderHeight)
{
    m_TargetBuffer = LoadRenderTexture(m_RenderWidth, m_RenderHeight);
    SetTextureFilter(m_TargetBuffer.texture, TEXTURE_FILTER_POINT);
    Logger::Info("PixelRenderer offscreen buffer initialized: {}x{}", m_RenderWidth, m_RenderHeight);
}

PixelRenderer::~PixelRenderer() {
    UnloadRenderTexture(m_TargetBuffer);
    Logger::Info("PixelRenderer unloaded offscreen texture.");
}

void PixelRenderer::BeginPixelMode() {
    BeginTextureMode(m_TargetBuffer);
    ClearBackground(BLACK);
}

void PixelRenderer::EndPixelMode() {
    EndTextureMode();
}

} // namespace Roguelike
