#include "BananaSDK/MultiDropdown.h"
#include "BananaSDK/UI.h"
#include <algorithm>
#include <cmath>

MultiDropdown::MultiDropdown(float x, float y, float w, float h)
    : m_X(x), m_Y(y), m_W(w), m_H(h) {}

void MultiDropdown::SetPosition(float x, float y) { m_X=x; m_Y=y; }
void MultiDropdown::SetSize(float w, float h)     { m_W=w; m_H=h; }
void MultiDropdown::SetRadius(float r)            { m_Radius = r; }
void MultiDropdown::SetFontSize(float s)          { m_FontSize = s; }
void MultiDropdown::SetPlaceholder(const std::string& t) { m_Placeholder = t; }
void MultiDropdown::SetBgColor(float r,float g,float b,float a)    { m_BgR=r; m_BgG=g; m_BgB=b; m_BgA=a; }
void MultiDropdown::SetPanelColor(float r,float g,float b,float a) { m_PanelR=r; m_PanelG=g; m_PanelB=b; m_PanelA=a; }
void MultiDropdown::SetTextColor(float r,float g,float b,float a)  { m_TextR=r; m_TextG=g; m_TextB=b; m_TextA=a; }
void MultiDropdown::SetAccentColor(float r,float g,float b,float a){ m_AccR=r; m_AccG=g; m_AccB=b; m_AccA=a; }
void MultiDropdown::SetOnChange(std::function<void(const std::vector<int>&)> cb) { m_OnChange = cb; }

void MultiDropdown::AddOption(const std::string& label) {
    m_Options.push_back(label);
    m_SelectedFlags.push_back(false);
    m_AnimT.push_back(0.0f);
}

void MultiDropdown::SetSelected(int index, bool selected) {
    if (index >= 0 && index < (int)m_SelectedFlags.size())
        m_SelectedFlags[index] = selected;
}

bool MultiDropdown::IsSelected(int index) const {
    if (index >= 0 && index < (int)m_SelectedFlags.size())
        return m_SelectedFlags[index];
    return false;
}

std::vector<int> MultiDropdown::GetSelected() const {
    std::vector<int> result;
    for (size_t i = 0; i < m_SelectedFlags.size(); i++)
        if (m_SelectedFlags[i]) result.push_back((int)i);
    return result;
}

float MultiDropdown::_RowHeight() const { return m_H; }

float MultiDropdown::_PanelHeight() const {
    return (float)m_Options.size() * _RowHeight();
}

std::string MultiDropdown::_SummaryText() const {
    auto sel = GetSelected();
    if (sel.empty()) return m_Placeholder;
    if (sel.size() == 1) return m_Options[sel[0]];
    return std::to_string(sel.size()) + " selected";
}

bool MultiDropdown::HitTest(float x, float y) const {
    if (x >= m_X && x <= m_X + m_W && y >= m_Y && y <= m_Y + m_H) return true;
    if (m_Open) {
        float panelY = m_Y + m_H;
        if (x >= m_X && x <= m_X + m_W && y >= panelY && y <= panelY + _PanelHeight()) return true;
    }
    return false;
}

bool MultiDropdown::OnTouch(float x, float y) {
    bool onMain = (x >= m_X && x <= m_X + m_W && y >= m_Y && y <= m_Y + m_H);

    if (m_Open) {
        float panelY = m_Y + m_H;
        bool onPanel = (x >= m_X && x <= m_X + m_W && y >= panelY && y <= panelY + _PanelHeight());
        if (onPanel) { m_IsDown = true; return true; }

        m_Open = false; // outside click closes
        return onMain;
    }

    if (onMain) { m_IsDown = true; return true; }
    return false;
}

void MultiDropdown::OnTouchMove(float x, float y) {}

void MultiDropdown::OnRelease(float x, float y) {
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
                m_SelectedFlags[row] = !m_SelectedFlags[row];
                if (m_OnChange) m_OnChange(GetSelected());
            }
            // Panel stays open for multi-select
        }
    }
}

void MultiDropdown::Draw(UIRenderer& ui) {
    auto now = std::chrono::steady_clock::now();
    float dt = m_FirstFrame ? 0.0f : std::chrono::duration<float>(now - m_LastTime).count();
    m_LastTime = now;
    m_FirstFrame = false;

    float r = std::min(m_Radius, std::min(m_W, m_H) * 0.5f);

    // Animate panel open/close
    float target = m_Open ? 1.0f : 0.0f;
    m_PanelAnimT += (target - m_PanelAnimT) * std::min(1.0f, dt * 14.0f);
    if (std::abs(target - m_PanelAnimT) < 0.01f) m_PanelAnimT = target;

    // Main box
    ui.DrawRect(m_X, m_Y, m_W, m_H, m_BgR, m_BgG, m_BgB, m_BgA, r);

    std::string summary = _SummaryText();
    float ty = m_Y + (m_H - m_FontSize) * 0.5f;
    ui.DrawText(m_X + 14.0f, ty, summary, m_TextR, m_TextG, m_TextB, m_TextA, m_FontSize);

    // Chevron
    {
        float cxv = m_X + m_W - 24.0f;
        float cyv = m_Y + m_H * 0.5f;
        float size = 14.0f;
        float stroke = 3.0f;
        float flip = 1.0f - 2.0f * m_PanelAnimT;
        float topY = cyv - size * 0.25f * flip;
        float botY = cyv + size * 0.25f * flip;
        ui.DrawRect(cxv - size*0.5f, std::min(topY,botY), size, std::abs(botY-topY) + stroke, m_TextR, m_TextG, m_TextB, m_TextA, stroke*0.5f);
    }

    // Panel
    if (m_PanelAnimT > 0.01f) {
        float fullH  = _PanelHeight();
        float panelH = fullH * m_PanelAnimT;
        float panelY = m_Y + m_H;

        ui.PushRoundedScissor(m_X, panelY, m_W, panelH, r);
        ui.DrawRect(m_X, panelY, m_W, fullH, m_PanelR, m_PanelG, m_PanelB, m_PanelA * m_PanelAnimT, r);

        float boxSize = m_H * 0.5f;
        float boxR    = boxSize * 0.25f;

        for (size_t i = 0; i < m_Options.size(); i++) {
            float rowY = panelY + (float)i * _RowHeight();
            float rowTy = rowY + (_RowHeight() - m_FontSize) * 0.5f;

            // Animate per-row checkbox
            float t = m_SelectedFlags[i] ? 1.0f : 0.0f;
            m_AnimT[i] += (t - m_AnimT[i]) * std::min(1.0f, dt * 14.0f);
            if (std::abs(t - m_AnimT[i]) < 0.01f) m_AnimT[i] = t;

            float boxX = m_X + 14.0f;
            float boxY = rowY + (_RowHeight() - boxSize) * 0.5f;

            // Checkbox square
            ui.DrawRect(boxX, boxY, boxSize, boxSize, 0.80f, 0.80f, 0.82f, 1.0f, boxR);
            if (m_AnimT[i] > 0.01f) {
                ui.DrawRect(boxX, boxY, boxSize, boxSize, m_AccR, m_AccG, m_AccB, m_AnimT[i], boxR);
                float csize = boxSize * m_AnimT[i];
                float cx = boxX + (boxSize - csize) * 0.5f;
                float cy = boxY + (boxSize - csize) * 0.5f;
                ui.DrawCheck(cx, cy, csize, boxSize * 0.12f, 1.0f, 1.0f, 1.0f, m_AnimT[i]);
            }

            // Label
            ui.DrawText(boxX + boxSize + 10.0f, rowTy, m_Options[i], m_TextR, m_TextG, m_TextB, m_TextA * m_PanelAnimT, m_FontSize);
        }
        ui.PopRoundedScissor();
    }
}
