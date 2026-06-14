#include "BananaSDK/Dropdown.h"
#include "BananaSDK/UI.h"
#include <algorithm>
#include <cmath>

Dropdown::Dropdown(float x, float y, float w, float h)
    : m_X(x), m_Y(y), m_W(w), m_H(h) {}

void Dropdown::SetPosition(float x, float y) { m_X=x; m_Y=y; }
void Dropdown::SetSize(float w, float h)     { m_W=w; m_H=h; }
void Dropdown::SetRadius(float r)            { m_Radius = r; }
void Dropdown::SetFontSize(float s)          { m_FontSize = s; }
void Dropdown::SetMaxVisibleRows(int rows)   { m_MaxRows = rows; }
void Dropdown::SetBgColor(float r,float g,float b,float a)    { m_BgR=r; m_BgG=g; m_BgB=b; m_BgA=a; }
void Dropdown::SetPanelColor(float r,float g,float b,float a) { m_PanelR=r; m_PanelG=g; m_PanelB=b; m_PanelA=a; }
void Dropdown::SetTextColor(float r,float g,float b,float a)  { m_TextR=r; m_TextG=g; m_TextB=b; m_TextA=a; }
void Dropdown::SetAccentColor(float r,float g,float b,float a){ m_AccR=r; m_AccG=g; m_AccB=b; m_AccA=a; }
void Dropdown::SetOnChange(std::function<void(int)> cb) { m_OnChange = cb; }

void Dropdown::AddOption(const std::string& label) { m_Options.push_back(label); }
void Dropdown::SetSelected(int index) { m_Selected = index; }

std::string Dropdown::GetSelectedLabel() const {
    if (m_Selected >= 0 && m_Selected < (int)m_Options.size())
        return m_Options[m_Selected];
    return "";
}

float Dropdown::_RowHeight() const { return m_H; }

float Dropdown::_PanelHeight() const {
    int visible = std::min((int)m_Options.size(), m_MaxRows);
    return visible * _RowHeight();
}

bool Dropdown::HitTest(float x, float y) const {
    if (x >= m_X && x <= m_X + m_W && y >= m_Y && y <= m_Y + m_H) return true;
    if (m_Open) {
        float panelY = m_Y + m_H;
        float panelH = _PanelHeight();
        if (x >= m_X && x <= m_X + m_W && y >= panelY && y <= panelY + panelH) return true;
    }
    return false;
}

bool Dropdown::OnTouch(float x, float y) {
    bool onMain = (x >= m_X && x <= m_X + m_W && y >= m_Y && y <= m_Y + m_H);

    if (m_Open) {
        float panelY = m_Y + m_H;
        float panelH = _PanelHeight();
        bool onPanel = (x >= m_X && x <= m_X + m_W && y >= panelY && y <= panelY + panelH);

        if (onPanel) {
            m_IsDown = true;
            return true;
        }

        // Clicked outside (including main box) -> close
        m_Open = false;
        // If they clicked the main box, treat as a toggle-close (don't reopen same frame)
        return onMain;
    }

    if (onMain) {
        m_IsDown = true;
        return true;
    }
    return false;
}

void Dropdown::OnTouchMove(float x, float y) {
    // no drag behavior needed
}

void Dropdown::OnRelease(float x, float y) {
    if (!m_IsDown) { m_IsDown = false; return; }
    m_IsDown = false;

    bool onMain = (x >= m_X && x <= m_X + m_W && y >= m_Y && y <= m_Y + m_H);

    if (!m_Open && onMain) {
        m_Open = true;
        return;
    }

    if (m_Open) {
        float panelY = m_Y + m_H;
        bool onPanel = (x >= m_X && x <= m_X + m_W && y >= panelY && y <= panelY + _PanelHeight());
        if (onPanel) {
            int row = (int)((y - panelY) / _RowHeight());
            if (row >= 0 && row < (int)m_Options.size()) {
                m_Selected = row;
                if (m_OnChange) m_OnChange(m_Selected);
            }
            m_Open = false;
        }
    }
}

void Dropdown::Draw(UIRenderer& ui) {
    auto now = std::chrono::steady_clock::now();
    float dt = m_FirstFrame ? 0.0f : std::chrono::duration<float>(now - m_LastTime).count();
    m_LastTime = now;
    m_FirstFrame = false;

    float r = std::min(m_Radius, std::min(m_W, m_H) * 0.5f);

    // Animate open/close
    float target = m_Open ? 1.0f : 0.0f;
    m_AnimT += (target - m_AnimT) * std::min(1.0f, dt * 14.0f);
    if (std::abs(target - m_AnimT) < 0.01f) m_AnimT = target;

    // Main box
    ui.DrawRect(m_X, m_Y, m_W, m_H, m_BgR, m_BgG, m_BgB, m_BgA, r);

    // Selected label
    std::string label = GetSelectedLabel();
    if (!label.empty()) {
        float ty = m_Y + (m_H - m_FontSize) * 0.5f;
        ui.DrawText(m_X + 14.0f, ty, label, m_TextR, m_TextG, m_TextB, m_TextA, m_FontSize);
    }

    // Chevron (animated rotation via morphing two triangles -> draw as small check-like arrow)
    {
        float cxv = m_X + m_W - 24.0f;
        float cyv = m_Y + m_H * 0.5f;
        float size = 14.0f;
        float stroke = 3.0f;
        // Rotate visually by flipping vertical offset based on m_AnimT (down-arrow -> up-arrow)
        float flip = 1.0f - 2.0f * m_AnimT; // 1 -> -1
        float topY = cyv - size * 0.25f * flip;
        float botY = cyv + size * 0.25f * flip;
        ui.DrawRect(cxv - size*0.5f, std::min(topY,botY), size, std::abs(botY-topY) + stroke, m_TextR, m_TextG, m_TextB, m_TextA, stroke*0.5f);
    }

    // Panel (animated height)
    if (m_AnimT > 0.01f) {
        float fullH = _PanelHeight();
        float panelH = fullH * m_AnimT;
        float panelY = m_Y + m_H;

        ui.PushRoundedScissor(m_X, panelY, m_W, panelH, r);
        ui.DrawRect(m_X, panelY, m_W, fullH, m_PanelR, m_PanelG, m_PanelB, m_PanelA * m_AnimT, r);

        for (size_t i = 0; i < m_Options.size(); i++) {
            float rowY = panelY + (float)i * _RowHeight();
            if ((int)i == m_Selected)
                ui.DrawRect(m_X, rowY, m_W, _RowHeight(), m_AccR, m_AccG, m_AccB, 0.15f * m_AnimT, 0.0f);

            float ty = rowY + (_RowHeight() - m_FontSize) * 0.5f;
            ui.DrawText(m_X + 14.0f, ty, m_Options[i], m_TextR, m_TextG, m_TextB, m_TextA * m_AnimT, m_FontSize);
        }
        ui.PopRoundedScissor();
    }
}
