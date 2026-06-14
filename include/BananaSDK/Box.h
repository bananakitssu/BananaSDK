#pragma once
class UIRenderer;

class Box {
public:
    Box() = default;
    Box(float x, float y, float w, float h);
    bool HitTest(float x, float y) const;
    void OnTouch(float x, float y);
    void OnTouchMove(float x, float y);
    void OnRelease(float x, float y);
    void SetColor(float r, float g, float b, float a = 1.0f);
    void SetRadius(float radius);
    void SetPosition(float x, float y);
    void SetSize(float w, float h);
    void Draw(UIRenderer& ui);
    float GetX() const { return m_X; }
    float GetY() const { return m_Y; }
    float GetW() const { return m_W; }
    float GetH() const { return m_H; }
private:
    float m_X=0, m_Y=0, m_W=0, m_H=0;
    float m_R=1, m_G=1, m_B=1, m_A=1;
    float m_Radius=0;
};
