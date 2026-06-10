#pragma once
#include <string>
#include <vector>

class UIRenderer;

class Text {
public:
    Text() = default;
    Text(float x, float y, const std::string& text,
         float maxWidth = 0.0f, float fontSize = 32.0f);

    bool HitTest(float x, float y) const;
    void OnTouch(float x, float y);
    void OnTouchMove(float x, float y);
    void OnRelease(float x, float y);
    void SetPosition(float x, float y);
    void SetText(const std::string& text);
    void SetFontSize(float size);
    void SetColor(float r, float g, float b, float a = 1.0f);
    void SetMaxWidth(float maxWidth);   // 0 = no wrapping
    void SetLineSpacing(float spacing);

    float GetHeight(UIRenderer& ui);    // total height after wrapping
    std::vector<std::string> WrapText(UIRenderer& ui);

    void Draw(UIRenderer& ui);

private:
    float m_X = 0, m_Y = 0;
    float m_MaxWidth    = 0.0f;
    float m_FontSize    = 32.0f;
    float m_LineSpacing = 4.0f;
    std::string m_Text;
    float m_R = 0.0f, m_G = 0.0f, m_B = 0.0f, m_A = 1.0f;
};
