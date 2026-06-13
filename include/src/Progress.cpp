#include "BananaSDK/Progress.h"
#include "BananaSDK/UI.h"
#include <cmath>
#include <algorithm>

Progress::Progress(float x, float y, float w, float h)
    : m_X(x), m_Y(y), m_W(w), m_H(h) {}

void Progress::SetPosition(float x, float y) { m_X=x; m_Y=y; }
void Progress::SetSize(float w, float h)     { m_W=w; m_H=h; }
void Progress::SetPadding(float p)           { m_Padding = p; }
void Progress::SetMorphPoint(float p)        { m_MorphPoint = std::clamp(p, 0.01f, 0.99f); }
void Progress::SetAnimationSpeed(float s)    { m_Speed = s; }
void Progress::SetTrackColor(float r,float g,float b,float a) { m_TrackR=r; m_TrackG=g; m_TrackB=b; m_TrackA=a; }
void Progress::SetFillColor(float r,float g,float b,float a)  { m_FillR=r; m_FillG=g; m_FillB=b; m_FillA=a; }
void Progress::SetProgress(float p) { m_Progress = p; }
float Progress::GetProgress() const { return m_Progress; }

static float smoothstep01(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return t*t*(3.0f - 2.0f*t);
}

void Progress::Draw(UIRenderer& ui) {
    auto now = std::chrono::steady_clock::now();
    float dt = m_FirstFrame ? 0.0f : std::chrono::duration<float>(now - m_LastTime).count();
    m_LastTime  = now;
    m_FirstFrame = false;

    float trackX = m_X + m_Padding;
    float trackW = std::max(0.0f, m_W - m_Padding * 2.0f);
    float trackH = m_H;
    float trackY = m_Y;
    float trackR = trackH * 0.5f;

    ui.DrawRect(trackX, trackY, trackW, trackH, m_TrackR, m_TrackG, m_TrackB, m_TrackA, trackR);

    if (m_Progress < 0.0f) {
        // Indeterminate loading animation — sliding pill, eased back and forth
        float cycle = 1.6f;
        m_IndetPhase += dt / cycle;
        while (m_IndetPhase > 1.0f) m_IndetPhase -= 1.0f;

        float fillW = trackW * 0.4f;
        float ease  = (m_IndetPhase < 0.5f)
            ? smoothstep01(m_IndetPhase * 2.0f)
            : 1.0f - smoothstep01((m_IndetPhase - 0.5f) * 2.0f);

        float fillX = trackX + (trackW - fillW) * ease;
        ui.DrawRect(fillX, trackY, fillW, trackH, m_FillR, m_FillG, m_FillB, m_FillA, trackR);
        return;
    }

    // Smoothly animate towards target progress
    float target = std::clamp(m_Progress, 0.0f, 1.0f);
    m_Display += (target - m_Display) * std::min(1.0f, dt * m_Speed);
    if (std::abs(target - m_Display) < 0.001f) m_Display = target;

    float p = m_Display;
    float fillW, fillH, fillRad, fillX, fillY;

    if (p <= m_MorphPoint) {
        // Dot growing into a circle
        float t = p / m_MorphPoint;
        fillW   = trackH * t;
        fillH   = fillW;
        fillRad = fillW * 0.5f;
        fillX   = trackX;
        fillY   = trackY + (trackH - fillH) * 0.5f;
    } else {
        // Circle morphed into a pill — now grows by width
        float t = (p - m_MorphPoint) / (1.0f - m_MorphPoint);
        fillW   = trackH + (trackW - trackH) * t;
        fillH   = trackH;
        fillRad = trackH * 0.5f;
        fillX   = trackX;
        fillY   = trackY;
    }

    if (fillW > 0.01f)
        ui.DrawRect(fillX, fillY, fillW, fillH, m_FillR, m_FillG, m_FillB, m_FillA, fillRad);
}
