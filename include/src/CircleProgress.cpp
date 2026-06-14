#include "BananaSDK/CircleProgress.h"
#include "BananaSDK/UI.h"
#include <cmath>
#include <algorithm>

CircleProgress::CircleProgress(float x, float y, float w, float h)
    : m_X(x), m_Y(y), m_W(w), m_H(h) {}

void CircleProgress::SetPosition(float x, float y) { m_X=x; m_Y=y; }
void CircleProgress::SetSize(float w, float h)     { m_W=w; m_H=h; }
void CircleProgress::SetStrokeWidth(float s)       { m_Stroke = s; }
void CircleProgress::SetAnimationSpeed(float s)    { m_Speed = s; }
void CircleProgress::SetCycleTime(float s)         { m_CycleTime = s; }
void CircleProgress::SetMorphTime(float s)         { m_MorphTime = s; }
void CircleProgress::SetTrackColor(float r,float g,float b,float a) { m_TrackR=r; m_TrackG=g; m_TrackB=b; m_TrackA=a; }
void CircleProgress::SetFillColor(float r,float g,float b,float a)  { m_FillR=r; m_FillG=g; m_FillB=b; m_FillA=a; }
void CircleProgress::SetProgress(float p) { m_Progress = p; }
float CircleProgress::GetProgress() const { return m_Progress; }

static float smoothstep01(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return t*t*(3.0f - 2.0f*t);
}

void CircleProgress::Draw(UIRenderer& ui) {
    auto now = std::chrono::steady_clock::now();
    float dt = m_FirstFrame ? 0.0f : std::chrono::duration<float>(now - m_LastTime).count();
    m_LastTime   = now;
    m_FirstFrame = false;

    const float TWO_PI = 6.28318530718f;
    const float TOP    = 0.0f;

    float cx = m_X + m_W * 0.5f;
    float cy = m_Y + m_H * 0.5f;
    float fullRadius = std::max(0.0f, std::min(m_W, m_H) * 0.5f - m_Stroke * 0.5f);

    // Morph: dot -> full ring (radius grows from 0)
    if (m_MorphElapsed < m_MorphTime) {
        m_MorphElapsed += dt;
        float t = smoothstep01(m_MorphElapsed / m_MorphTime);
        float radius = fullRadius * t;

        ui.DrawRing(cx, cy, radius, m_Stroke, 0.0f, TWO_PI, m_TrackR, m_TrackG, m_TrackB, m_TrackA);
        ui.DrawRing(cx, cy, radius, m_Stroke, 0.0f, TWO_PI, m_FillR,  m_FillG,  m_FillB,  m_FillA);
        return;
    }

    // Track
    ui.DrawRing(cx, cy, fullRadius, m_Stroke, 0.0f, TWO_PI, m_TrackR, m_TrackG, m_TrackB, m_TrackA);

    if (m_Progress < 0.0f) {
        // Material-style indeterminate spinner
        m_RotOffset += dt * 2.0f;
        while (m_RotOffset > TWO_PI) m_RotOffset -= TWO_PI;

        m_ArcPhase += dt / m_CycleTime;
        while (m_ArcPhase > 1.0f) m_ArcPhase -= 1.0f;

        float minSweep = 0.35f;
        float maxSweep = 5.0f;
        float sweep, startAngle;

        if (m_ArcPhase < 0.5f) {
            float t = smoothstep01(m_ArcPhase / 0.5f);
            sweep      = minSweep + (maxSweep - minSweep) * t;
            startAngle = m_RotOffset;
        } else {
            float t = smoothstep01((m_ArcPhase - 0.5f) / 0.5f);
            sweep      = maxSweep - (maxSweep - minSweep) * t;
            startAngle = m_RotOffset + (maxSweep - minSweep) * t;
        }

        startAngle = std::fmod(startAngle, TWO_PI);
        if (startAngle < 0.0f) startAngle += TWO_PI;

        ui.DrawRing(cx, cy, fullRadius, m_Stroke, startAngle, sweep, m_FillR, m_FillG, m_FillB, m_FillA);
        return;
    }

    // Determinate: fill clockwise from top
    float target = std::clamp(m_Progress, 0.0f, 1.0f);
    m_Display += (target - m_Display) * std::min(1.0f, dt * m_Speed);
    if (std::abs(target - m_Display) < 0.001f) m_Display = target;

    float sweep = m_Display * TWO_PI;
    if (sweep > 0.001f)
        ui.DrawRing(cx, cy, fullRadius, m_Stroke, TOP, sweep, m_FillR, m_FillG, m_FillB, m_FillA);
}
