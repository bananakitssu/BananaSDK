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
    float h         = std::max(0.0f, m_H - m_Padding * 2.0f);  // full padded height (circle/pill height)

    if (m_Progress < 0.0f) {
        // 5-phase indeterminate loop:
        // grow (point->circle) -> widen (circle->limit) -> slide -> shrink width (limit->circle) -> shrink to nothing
        m_IndetPhase += dt / m_CycleTime;
        while (m_IndetPhase > 1.0f) m_IndetPhase -= 1.0f;
        float ph = m_IndetPhase;

        float growLimit = usableW * m_GrowLimitF;

        float f1 = 0.15f; // grow to circle
        float f2 = 0.15f; // widen to limit
        float f3 = 0.30f; // slide
        float f4 = 0.15f; // shrink width back to circle
        float f5 = 0.25f; // shrink to nothing
        float t1 = f1, t2 = t1+f2, t3 = t2+f3, t4 = t3+f4;

        float fillW, fillH, fillX;

        if (ph < t1) {
            float t = smoothstep01(ph / f1);
            fillW = h * t;
            fillH = fillW;
            fillX = leftEdge;
        } else if (ph < t2) {
            float t = smoothstep01((ph - t1) / f2);
            fillH = h;
            fillW = h + (growLimit - h) * t;
            fillX = leftEdge;
        } else if (ph < t3) {
            float t = smoothstep01((ph - t2) / f3);
            fillH = h;
            fillW = growLimit;
            fillX = leftEdge + (rightEdge - growLimit - leftEdge) * t;
        } else if (ph < t4) {
            float t = smoothstep01((ph - t3) / f4);
            fillH = h;
            fillW = growLimit + (h - growLimit) * t;
            fillX = rightEdge - fillW;
        } else {
            float t = smoothstep01((ph - t4) / f5);
            fillH = h * (1.0f - t);
            fillW = fillH;
            fillX = rightEdge - fillW;
        }

        float fillY    = m_Y + (m_H - fillH) * 0.5f;
        float fillRad  = std::min(fillW, fillH) * 0.5f;

        if (fillW > 0.01f && fillH > 0.01f)
            ui.DrawRect(fillX, fillY, fillW, fillH, m_FillR, m_FillG, m_FillB, m_FillA, fillRad);
        return;
    }

    // Determinate: dot -> circle -> pill -> bar
    float target = std::clamp(m_Progress, 0.0f, 1.0f);
    m_Display += (target - m_Display) * std::min(1.0f, dt * m_Speed);
    if (std::abs(target - m_Display) < 0.001f) m_Display = target;

    float p = m_Display;
    float fillW, fillH, fillX, fillY, fillRad;

    if (p <= m_MorphPoint) {
        float t = p / m_MorphPoint;
        fillH = h * t;
        fillW = fillH;
        fillX = leftEdge;
        fillY = m_Y + (m_H - fillH) * 0.5f;
        fillRad = fillH * 0.5f;
    } else {
        float t = (p - m_MorphPoint) / (1.0f - m_MorphPoint);
        fillH = h;
        fillW = h + (usableW - h) * t;
        fillX = leftEdge;
        fillY = m_Y + m_Padding;
        fillRad = h * 0.5f;
    }

    if (fillW > 0.01f && fillH > 0.01f)
        ui.DrawRect(fillX, fillY, fillW, fillH, m_FillR, m_FillG, m_FillB, m_FillA, fillRad);
}
