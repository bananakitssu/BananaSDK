#include "BananaSDK/RadioGroup.h"
#include "BananaSDK/UI.h"
#include <algorithm>
#include <cmath>

RadioGroup::RadioGroup(float x, float y, float w, float h)
    : m_X(x), m_Y(y), m_W(w), m_H(h) {}

void RadioGroup::SetPosition(float x, float y) { m_X=x; m_Y=y; }
void RadioGroup::SetSize(float w, float h)     { m_W=w; m_H=h; }
void RadioGroup::SetRadius(float r)            { m_Radius = r; }
void RadioGroup::SetPadding(float p)           { m_Padding = p; }
void RadioGroup::SetRowHeight(float h)         { m_RowHeight = h; }
void RadioGroup::SetFontSize(float s)          { m_FontSize = s; }
void RadioGroup::SetBgColor(float r,float g,float b,float a)       { m_BgR=r; m_BgG=g; m_BgB=b; m_BgA=a; }
void RadioGroup::SetScrollbarColor(float r,float g,float b,float a){ m_SbR=r; m_SbG=g; m_SbB=b; m_SbA=a; }
void RadioGroup::SetRadioColor(float r,float g,float b,float a)    { m_RadioR=r; m_RadioG=g; m_RadioB=b; m_RadioA=a; }
void RadioGroup::SetTextColor(float r,float g,float b,float a)     { m_TextR=r; m_TextG=g; m_TextB=b; m_TextA=a; }
void RadioGroup::SetOnChange(std::function<void(int)> cb) { m_OnChange = cb; }

void RadioGroup::AddOption(const std::string& label) {
    m_Options.push_back(label);
}

void RadioGroup::SetSelected(int index) {
    m_Selected = index;
}

float RadioGroup::_ContentH() const {
    return (float)m_Options.size() * m_RowHeight;
}

float RadioGroup::_MaxScroll() const {
    return std::max(0.0f, (_ContentH() + m_Padding) - m_H);
}

const float RG_SCROLLBAR_WIDTH = 8.0f;
const float RG_SCROLLBAR_MARGIN = 4.0f;

void RadioGroup::_GetThumbRect(float& tx, float& ty, float& tw, float& th) const {
    float contentH  = _ContentH();
    float maxScroll = _MaxScroll();
    tw = RG_SCROLLBAR_WIDTH;
    tx = m_X + m_W - RG_SCROLLBAR_WIDTH - RG_SCROLLBAR_MARGIN;

    if (contentH <= m_H || contentH <= 0.0f) {
        th = m_H;
        ty = m_Y;
        return;
    }

    th = std::max(20.0f, m_H * (m_H / contentH));
    float trackH = m_H - th;
    float t = (maxScroll > 0.0f) ? (m_ScrollOffset / maxScroll) : 0.0f;
    ty = m_Y + trackH * t;
}

bool RadioGroup::HitTest(float x, float y) const {
    return x >= m_X && x <= m_X + m_W && y >= m_Y && y <= m_Y + m_H;
}

bool RadioGroup::OnTouch(float x, float y) {
    if (!HitTest(x, y)) return false;

    float tx, ty, tw, th;
    _GetThumbRect(tx, ty, tw, th);
    bool onThumb = (x >= tx && x <= tx + tw && y >= ty && y <= ty + th) && (_ContentH() > m_H);

    m_IsDown        = true;
    m_IsDragging    = false;
    m_DraggingThumb = onThumb;
    m_TouchStartY   = y;
    m_ScrollStart   = m_ScrollOffset;

    return true;
}

void RadioGroup::OnTouchMove(float x, float y) {
    if (!m_IsDown) return;

    float dy = m_TouchStartY - y;

    if (m_DraggingThumb) {
        float maxScroll = _MaxScroll();
        float contentH  = _ContentH();
        float trackH = m_H - std::max(20.0f, m_H * (m_H / std::max(contentH, m_H)));
        if (trackH > 0.0f) {
            float scrollDelta = (dy * -1.0f) * (maxScroll / trackH);
            m_ScrollOffset = std::clamp(m_ScrollStart - scrollDelta, 0.0f, maxScroll);
        }
        return;
    }

    if (std::abs(dy) > 8.0f) m_IsDragging = true;

    if (m_IsDragging) {
        float maxScroll = _MaxScroll();
        m_ScrollOffset = std::clamp(m_ScrollStart + dy, 0.0f, maxScroll);
    }
}

void RadioGroup::OnRelease(float x, float y) {
    if (!m_IsDown) return;

    if (!m_DraggingThumb && !m_IsDragging) {
        // Determine which row was tapped
        float localY = y - m_Y + m_ScrollOffset - m_Padding;
        if (localY >= 0.0f) {
            int index = (int)(localY / m_RowHeight);
            if (index >= 0 && index < (int)m_Options.size()) {
                m_Selected = index;
                if (m_OnChange) m_OnChange(m_Selected);
            }
        }
    }

    m_IsDown        = false;
    m_IsDragging    = false;
    m_DraggingThumb = false;
}

void RadioGroup::Draw(UIRenderer& ui) {
    ui.DrawRect(m_X, m_Y, m_W, m_H, m_BgR, m_BgG, m_BgB, m_BgA, m_Radius);

    ui.PushRoundedScissor(m_X, m_Y, m_W, m_H, m_Radius);

    const float TWO_PI = 6.28318530718f;
    float radioR = m_RowHeight * 0.28f;
    float radioStroke = 3.0f;

    for (size_t i = 0; i < m_Options.size(); i++) {
        float rowY = m_Y + m_Padding + (float)i * m_RowHeight - m_ScrollOffset;
        if (rowY + m_RowHeight < m_Y) continue;
        if (rowY > m_Y + m_H) break;

        float cx = m_X + m_Padding + radioR;
        float cy = rowY + m_RowHeight * 0.5f;

        // Outer ring
        ui.DrawRing(cx, cy, radioR, radioStroke, 0.0f, TWO_PI, m_RadioR, m_RadioG, m_RadioB, m_RadioA);

        // Inner filled dot if selected
        if ((int)i == m_Selected) {
            float innerR = radioR * 0.5f;
            ui.DrawRing(cx, cy, innerR * 0.5f, innerR, 0.0f, TWO_PI, m_RadioR, m_RadioG, m_RadioB, m_RadioA);
        }

        // Label
        float textX = m_X + m_Padding + radioR * 2.0f + 12.0f;
        float textY = rowY + (m_RowHeight - m_FontSize) * 0.5f;
        ui.DrawText(textX, textY, m_Options[i], m_TextR, m_TextG, m_TextB, m_TextA, m_FontSize);
    }

    if (_ContentH() > m_H) {
        float tx, ty, tw, th;
        _GetThumbRect(tx, ty, tw, th);
        ui.DrawRect(tx, ty, tw, th, m_SbR, m_SbG, m_SbB, m_SbA, tw * 0.5f);
    }

    ui.PopRoundedScissor();
}
