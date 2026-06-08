#pragma once
class UIRenderer;

class Box {
public:
    Box() = default;
    Box(float x, float y, float w, float h);
    void SetColor(float r, float g, float b, float a = 1.0f);
    void SetRadius(float radius);
    void SetPosition(float x, float y);
    void SetSize(float w, float h);
    void Draw(UIRenderer& ui);
private:
    float m_X=0, m_Y=0, m_W=0, m_H=0;
    float m_R=1, m_G=1, m_B=1, m_A=1;
    float m_Radius=0;
};
