#pragma once
#include <string>
#include <functional>
#include <chrono>
#include <vector>

class UIRenderer;

class Button {
public:
    Button() = default;
    Button(float x, float y, float w, float h,
           const std::string& text = "", float fontSize = 32.0f);

    void SetColor(float r, float g, float b, float a = 1.0f);
    void SetTextColor(float r, float g, float b, float a = 1.0f);
    void SetRadius(float radius);
    void SetText(const std::string& text);
    void SetFontSize(float size);
    void SetTextWrap(bool wrap);        // wraps within button bounds
    void SetPosition(float x, float y);
    void SetSize(float w, float h);

    void SetOnClick(std::function<void()> cb);
    bool HitTest(float x, float y) const;
    bool OnTouch(float x, float y);
    void OnTouchMove(float x, float y);
    void OnRelease(float x, float y);

    float GetY() { return m_Y };
    float GetX() { return m_X };
    float GetW() { return m_W };
    float GetH() { return m_H };

    void Draw(UIRenderer& ui);

private:
    std::vector<std::string> _WrapText(UIRenderer& ui);

    float m_X = 0, m_Y = 0, m_W = 0, m_H = 0;
    std::string m_Text;
    float m_FontSize  = 32.0f;
    bool  m_TextWrap  = false;

    float m_R = 0.80f, m_G = 0.80f, m_B = 0.82f, m_A = 1.0f;
    float m_TR = 0.0f, m_TG = 0.478f, m_TB = 1.0f, m_TA = 1.0f;
    float m_Radius    = 9999.0f;
    bool  m_IsDown    = false;
    float m_PressAnim = 0.0f;
    std::chrono::steady_clock::time_point m_LastTime;

    std::function<void()> m_OnClick;
};
