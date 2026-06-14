#pragma once
#include <functional>

class UIRenderer;

class CheckBox {
public:
    CheckBox() = default;
    CheckBox(float x, float y, float size);

    void SetPosition(float x, float y);
    void SetSize(float size);
    void SetRadius(float radius);
    void SetChecked(bool checked);
    void SetOnChange(std::function<void(bool)> cb);
    void SetBoxColor(float r, float g, float b, float a = 1.0f);
    void SetFillColor(float r, float g, float b, float a = 1.0f);

    bool IsChecked() const { return m_Checked; }
    float GetX() const { return m_X; }
    float GetY() const { return m_Y; }
    float GetW() const { return m_W; }
    float GetH() const { return m_H; }

    bool HitTest(float x, float y) const;
    bool OnTouch(float x, float y);
    void OnTouchMove(float x, float y);
    void OnRelease(float x, float y);

    void Draw(UIRenderer& ui);

private:
    float m_X=0, m_Y=0, m_W=32.0f, m_H=32.0f;
    float m_Radius = 8.0f;
    bool  m_Checked = false;
    bool  m_IsDown  = false;

    float m_BoxR=0.80f, m_BoxG=0.80f, m_BoxB=0.82f, m_BoxA=1.0f;
    float m_FillR=0.0f, m_FillG=0.478f, m_FillB=1.0f, m_FillA=1.0f;

    std::function<void(bool)> m_OnChange;
};
