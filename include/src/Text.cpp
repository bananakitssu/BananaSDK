#include "BananaSDK/Text.h"
#include "BananaSDK/UI.h"
#include <sstream>

Text::Text(float x, float y, const std::string& text, float maxWidth, float fontSize)
    : m_X(x), m_Y(y), m_Text(text), m_MaxWidth(maxWidth), m_FontSize(fontSize) {}

bool Text::HitTest(float x, float y) const { return false; }
void Text::OnTouch(float x, float y)       {}
void Text::OnTouchMove(float x, float y)   {}
void Text::OnRelease(float x, float y)     {}
void Text::SetPosition(float x, float y)                { m_X = x; m_Y = y; }
void Text::SetText(const std::string& text)              { m_Text = text; }
void Text::SetFontSize(float size)                       { m_FontSize = size; }
void Text::SetColor(float r, float g, float b, float a)  { m_R=r; m_G=g; m_B=b; m_A=a; }
void Text::SetMaxWidth(float maxWidth)                   { m_MaxWidth = maxWidth; }
void Text::SetLineSpacing(float spacing)                 { m_LineSpacing = spacing; }

std::vector<std::string> Text::WrapText(UIRenderer& ui) {
    std::vector<std::string> lines;

    if (m_MaxWidth <= 0.0f || m_Text.empty()) {
        lines.push_back(m_Text);
        return lines;
    }

    std::istringstream stream(m_Text);
    std::string word, currentLine;

    while (stream >> word) {
        std::string candidate = currentLine.empty() ? word : currentLine + " " + word;

        if (ui.MeasureText(candidate, m_FontSize) > m_MaxWidth && !currentLine.empty()) {
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

float Text::GetHeight(UIRenderer& ui) {
    auto lines = WrapText(ui);
    return (float)lines.size() * (m_FontSize + m_LineSpacing);
}

void Text::Draw(UIRenderer& ui) {
    auto lines = WrapText(ui);
    float y = m_Y;
    for (auto& line : lines) {
        ui.DrawText(m_X, y, line, m_R, m_G, m_B, m_A, m_FontSize);
        y += m_FontSize + m_LineSpacing;
    }
}
