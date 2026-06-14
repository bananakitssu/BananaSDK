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

    float cx = m_X + m_W * 0.5f;
    float cy = m_Y + m_H * 0.5f;
    float radius = std::max(0.0f, std::min(m_W, m_H) * 0.5f - m_Stroke * 0.5f);

    // Track ring is always visible
    ui.DrawRing(cx, cy, radius, m_Stroke, 0.0f, TWO_PI, m_TrackR, m_TrackG, m_TrackB, m_TrackA);

    const float minSweep = 0.35f;
    const float maxSweep = TWO_PI * 0.92f;

    if (m_Progress < 0.0f) {
        // Morph-in: grow from a dot to the oscillation's starting sweep (minSweep),
        // at startAngle=0, matching the oscillation's phase=0 state exactly.
        if (m_MorphElapsed < m_MorphTime) {
            m_MorphElapsed += dt;
            float t = smoothstep01(m_MorphElapsed / m_MorphTime);
            float sweep = minSweep * t;
            if (sweep > 0.001f)
                ui.DrawRing(cx, cy, radius, m_Stroke, 0.0f, sweep, m_FillR, m_FillG, m_FillB, m_FillA);
            return;
        }

        // Base rotation, continuously advancing (kept bounded for precision)
        m_RotOffset += dt * (TWO_PI / m_CycleTime);
        m_RotOffset = std::fmod(m_RotOffset, TWO_PI);
        if (m_RotOffset < 0.0f) m_RotOffset += TWO_PI;

        // Cycle phase, kept bounded
        m_ArcPhase = std::fmod(m_ArcPhase + dt, m_CycleTime);
        float phase = m_ArcPhase / m_CycleTime;

        // sweepVariablePart: 0 -> (maxSweep-minSweep) -> 0 over the cycle
        float C = (maxSweep - minSweep) * 0.5f;
        float sweepVar = C * (1.0f - std::cos(TWO_PI * phase));

        // First half (phase < 0.5): sweepVar increasing -> head speeds up, tail slows -> arc grows
        // Second half (phase > 0.5): sweepVar decreasing -> tail speeds up to catch head -> arc shrinks
        float tailAngle = m_RotOffset - sweepVar * 0.5f;
        float sweep     = sweepVar + minSweep;

        ui.DrawRing(cx, cy, radius, m_Stroke, tailAngle, sweep, m_FillR, m_FillG, m_FillB, m_FillA);
        return;
    }

    // Determinate: fill clockwise from top, rounded caps.
    float target = std::clamp(m_Progress, 0.0f, 1.0f);
    m_Display += (target - m_Display) * std::min(1.0f, dt * m_Speed);
    if (std::abs(target - m_Display) < 0.001f) m_Display = target;

    float sweep = m_Display * TWO_PI;
    if (sweep > 0.001f)
        ui.DrawRing(cx, cy, radius, m_Stroke, 0.0f, std::min(sweep, TWO_PI - 0.01f),
                     m_FillR, m_FillG, m_FillB, m_FillA);
}
