#pragma once
#include "UI.h"
#include <string>
#include <functional>

class Button {
public:
    Button() = default;
    Button(float x, float y, float w, float h, const std::string& text = "");

    // Styling
    void SetColor(float r, float g, float b, float a = 1.0f);
    void SetTextColor(float r, float g, float b, float a = 1.0f);
    void SetRadius(float radius);
    void SetText(const std::string& text);
    void SetFontSize(float size);
    void SetPosition(float x, float y);
    void SetSize(float w, float h);

    // Interaction
    void SetOnClick(std::function<void()> cb);
    bool OnTouch(float x, float y); // returns true if tapped inside

    // Rendering
    void Draw(UIRenderer& ui);

private:
    float m_X = 0, m_Y = 0, m_W = 0, m_H = 0;
    std::string m_Text;
    float m_FontSize = 32.0f;

    float m_R = 0.80f, m_G = 0.80f, m_B = 0.82f, m_A = 1.0f;
    float m_TR = 0.0f,   m_TG = 0.478f, m_TB = 1.0f,   m_TA = 1.0f;
    float m_Radius = 9999.0f;

    std::function<void()> m_OnClick;
};
