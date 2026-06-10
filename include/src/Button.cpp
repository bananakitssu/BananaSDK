#include "BananaSDK/Button.h"
#include "BananaSDK/UI.h"
#include <algorithm>
#include <sstream>

Button::Button(float x, float y, float w, float h, const std::string& text, float fontSize)
    : m_X(x), m_Y(y), m_W(w), m_H(h), m_Text(text), m_FontSize(fontSize) {}

bool Button::HitTest(float x, float y) const {
    return x >= m_X && x <= m_X + m_W && y >= m_Y && y <= m_Y + m_H;
}

void Button::SetColor(float r, float g, float b, float a)     { m_R=r; m_G=g; m_B=b; m_A=a; }
void Button::SetTextColor(float r, float g, float b, float a) { m_TR=r; m_TG=g; m_TB=b; m_TA=a; }
void Button::SetRadius(float radius)                          { m_Radius = radius; }
void Button::SetText(const std::string& text)                 { m_Text = text; }
void Button::SetFontSize(float size)                          { m_FontSize = size; }
void Button::SetTextWrap(bool wrap)                           { m_TextWrap = wrap; }
void Button::SetPosition(float x, float y)                    { m_X=x; m_Y=y; }
void Button::SetSize(float w, float h)                        { m_W=w; m_H=h; }
void Button::SetOnClick(std::function<void()> cb)             { m_OnClick = cb; }

bool Button::OnTouch(float x, float y) {
    if (x >= m_X && x <= m_X + m_W && y >= m_Y && y <= m_Y + m_H) {
        m_IsDown = true;
        return true;
    }
    return false;
}

void Button::OnTouchMove(float x, float y) {
    if (m_IsDown) {
        if (x < m_X || x > m_X + m_W || y < m_Y || y > m_Y + m_H)
            m_IsDown = false;  // finger left — cancel, animate back up
    }
}

void Button::OnRelease(float x, float y) {
    if (m_IsDown && x >= m_X && x <= m_X + m_W && y >= m_Y && y <= m_Y + m_H) {
        if (m_OnClick) m_OnClick();
    }
    m_IsDown = false;
}

std::vector<std::string> Button::_WrapText(UIRenderer& ui) {
    std::vector<std::string> lines;
    if (m_Text.empty()) return lines;

    float maxWidth = m_W - 24.0f;  // 12px padding each side

    if (!m_TextWrap || maxWidth <= 0.0f) {
        lines.push_back(m_Text);
        return lines;
    }

    std::istringstream stream(m_Text);
    std::string word, currentLine;

    while (stream >> word) {
        std::string candidate = currentLine.empty() ? word : currentLine + " " + word;
        if (ui.MeasureText(candidate, m_FontSize) > maxWidth && !currentLine.empty()) {
            lines.push_back(currentLine);
            currentLine = word;
        } else {
            currentLine = candidate;
        }
    }

    if (!currentLine.empty())
        lines.push_back(currentLine);

    return lines;
}

void Button::Draw(UIRenderer& ui) {
    auto now = std::chrono::steady_clock::now();
    float dt = std::chrono::duration<float>(now - m_LastTime).count();
    m_LastTime = now;

    float speed = 10.0f;
    if (m_IsDown)
        m_PressAnim = std::min(1.0f, m_PressAnim + dt * speed);
    else
        m_PressAnim = std::max(0.0f, m_PressAnim - dt * speed);

    float t  = m_PressAnim * 0.25f;
    float r  = std::min(m_Radius, std::min(m_W, m_H) / 2.0f);
    ui.DrawRect(m_X, m_Y, m_W, m_H, m_R - t, m_G - t, m_B - t, m_A, r);

    if (!m_Text.empty()) {
        auto lines   = _WrapText(ui);
        float lineH  = m_FontSize + 4.0f;
        float totalH = (float)lines.size() * lineH - 4.0f;
        float ty     = m_Y + (m_H - totalH) * 0.5f;

        for (auto& line : lines) {
            ui.DrawText(m_X + 12.0f, ty, line, m_TR, m_TG, m_TB, m_TA, m_FontSize);
            ty += lineH;
        }
    }
}
