#pragma once
#include <chrono>

class UIRenderer;

class Progress {
public:
    Progress() = default;
    Progress(float x, float y, float w, float h);

    void SetPosition(float x, float y);
    void SetSize(float w, float h);
    void SetPadding(float padding);          // inset of fill within track, default 24.0f
    void SetProgress(float progress);        // 0.0 - 1.0, or -1.0f for indeterminate
    void SetMorphPoint(float point);         // 0-1, default 0.15
    void SetGrowLimit(float fraction);       // 0-1 of usable width, default 0.4 (indeterminate)
    void SetAnimationSpeed(float speed);     // determinate lerp speed, default 8.0f
    void SetCycleTime(float seconds);        // indeterminate loop duration, default 1.6f
    void SetTrackColor(float r, float g, float b, float a = 1.0f);
    void SetFillColor(float r, float g, float b, float a = 1.0f);
    float GetProgress() const;
    float GetX() const { return m_X; }
    float GetY() const { return m_Y; }
    float GetW() const { return m_W; }
    float GetH() const { return m_H; }

    bool HitTest(float x, float y) const { return false; }
    void OnTouch(float x, float y) {}
    void OnTouchMove(float x, float y) {}
    void OnRelease(float x, float y) {}

    void Draw(UIRenderer& ui);

private:
    float m_X=0, m_Y=0, m_W=200.0f, m_H=24.0f;
    float m_Padding    = 24.0f;
    float m_MorphPoint = 0.15f;
    float m_GrowLimitF = 0.4f;
    float m_Speed      = 8.0f;
    float m_CycleTime  = 1.6f;

    float m_Progress = 0.0f;
    float m_Display  = 0.0f;

    float m_TrackR=0.85f, m_TrackG=0.85f, m_TrackB=0.87f, m_TrackA=1.0f;
    float m_FillR=1.0f,  m_FillG=0.45f, m_FillB=0.45f, m_FillA=1.0f;

    float m_IndetPhase = 0.0f;
    std::chrono::steady_clock::time_point m_LastTime;
    bool  m_FirstFrame = true;
};
