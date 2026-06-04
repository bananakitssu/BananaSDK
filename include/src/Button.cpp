#include "BananaSDK/Button.h"
#include <algorithm>

Button::Button(float x, float y, float w, float h, const std::string& text)
    : m_X(x), m_Y(y), m_W(w), m_H(h), m_Text(text) {}

void Button::SetColor(float r, float g, float b, float a)     { m_R=r; m_G=g; m_B=b; m_A=a; }
void Button::SetTextColor(float r, float g, float b, float a) { m_TR=r; m_TG=g; m_TB=b; m_TA=a; }
void Button::SetRadius(float radius)                          { m_Radius = radius; }
void Button::SetText(const std::string& text)                 { m_Text = text; }
void Button::SetFontSize(float size)                          { m_FontSize = size; }
void Button::SetPosition(float x, float y)                    { m_X=x; m_Y=y; }
void Button::SetSize(float w, float h)                        { m_W=w; m_H=h; }
void Button::SetOnClick(std::function<void()> cb)             { m_OnClick = cb; }

bool Button::OnTouch(float x, float y) {
    if (x >= m_X && x <= m_X + m_W && y >= m_Y && y <= m_Y + m_H) {
        if (m_OnClick) m_OnClick();
        return true;
    }
    return false;
}

void Button::Draw(UIRenderer& ui) {
    float r = std::min(m_Radius, std::min(m_W, m_H) / 2.0f);
    ui.DrawRect(m_X, m_Y, m_W, m_H, m_R, m_G, m_B, m_A, r);
    if (!m_Text.empty()) {
        float ty = m_Y + (m_H - m_FontSize) * 0.5f;
        ui.DrawText(m_X + 12.0f, ty, m_Text, m_TR, m_TG, m_TB, m_TA, m_FontSize);
    }
}
