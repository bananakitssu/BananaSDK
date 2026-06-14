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
void Progress::SetGrowLimit(float f)         { m_GrowLimitF = std::clamp(f, 0.05f, 0.95f); }
void Progress::SetAnimationSpeed(float s)    { m_Speed = s; }
void Progress::SetCycleTime(float s)         { m_CycleTime = s; }
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
    m_LastTime   = now;
    m_FirstFrame = false;

    float trackR = m_H * 0.5f;
    ui.DrawRect(m_X, m_Y, m_W, m_H, m_TrackR, m_TrackG, m_TrackB, m_TrackA, trackR);

    float leftEdge  = m_X + m_Padding;
    float rightEdge = m_X + m_W - m_Padding;
    float usableW   = std::max(0.0f, rightEdge - leftEdge);
    float h         = std::max(0.0f, m_H - m_Padding * 2.0f);  // constant fill height
    float fillY     = m_Y + m_Padding;
    float fillRad   = h * 0.5f;

    if (m_Progress < 0.0f) {
    m_IndetPhase += dt / m_CycleTime;
    while (m_IndetPhase > 1.0f) m_IndetPhase -= 1.0f;
    float ph = m_IndetPhase;

    float growLimit = usableW * m_GrowLimitF;

    float f0 = 0.10f; // grow from nothing -> circle (left-anchored)
    float f1 = 0.15f; // circle -> pill (left-anchored)
    float f2 = 0.40f; // slide pill right
    float f3 = 0.15f; // pill -> circle (right-anchored)
    float f4 = 0.20f; // circle -> nothing (right-anchored)
    float t0=f0, t1=t0+f1, t2=t1+f2, t3=t2+f3;

    float fillW, fillH, fillX, fillYDyn;

    if (ph < t0) {
    float t = smoothstep01(ph / f0);
    float centerX = leftEdge + h * 0.5f;
    fillH = h * t;
    fillW = fillH;
    fillX = centerX - fillW * 0.5f;
    fillYDyn = m_Y + (m_H - fillH) * 0.5f;
} else if (ph < t1) {
        float t = smoothstep01((ph - t0) / f1);
        fillH = h;
        fillW = h + (growLimit - h) * t;
        fillX = leftEdge;
    } else if (ph < t2) {
        float t = smoothstep01((ph - t1) / f2);
        fillH = h;
        fillW = growLimit;
        fillX = leftEdge + (rightEdge - growLimit - leftEdge) * t;
    } else if (ph < t3) {
        float t = smoothstep01((ph - t2) / f3);
        fillH = h;
        fillW = growLimit + (h - growLimit) * t;
        fillX = rightEdge - fillW;
    } else {
    float t = smoothstep01((ph - t3) / f4);
    float centerX = rightEdge - h * 0.5f;  // center of the circle from end of phase 3
    fillH = h * (1.0f - t);
    fillW = fillH;
    fillX = centerX - fillW * 0.5f;
    fillYDyn = m_Y + (m_H - fillH) * 0.5f;
}

    float fillYDyn = m_Y + (m_H - fillH) * 0.5f;
    float rad = std::min(fillW, fillH) * 0.5f;
    if (fillW > 0.01f && fillH > 0.01f)
        ui.DrawRect(fillX, fillYDyn, fillW, fillH, m_FillR, m_FillG, m_FillB, m_FillA, rad);
    return;
}

    // Determinate: dot -> pill -> bar
    float target = std::clamp(m_Progress, 0.0f, 1.0f);
    m_Display += (target - m_Display) * std::min(1.0f, dt * m_Speed);
    if (std::abs(target - m_Display) < 0.001f) m_Display = target;

    float p = m_Display;
    float fillW;

    if (p <= m_MorphPoint) {
        float t = p / m_MorphPoint;
        fillW = h * t;
    } else {
        float t = (p - m_MorphPoint) / (1.0f - m_MorphPoint);
        fillW = h + (usableW - h) * t;
    }

    if (fillW > 0.01f && h > 0.01f)
        ui.DrawRect(leftEdge, fillY, fillW, h, m_FillR, m_FillG, m_FillB, m_FillA, fillRad);
}
