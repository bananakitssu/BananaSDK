#include "BananaSDK/CheckBox.h"
#include "BananaSDK/UI.h"
#include <cmath>
#include <algorithm>

CheckBox::CheckBox(float x, float y, float size)
    : m_X(x), m_Y(y), m_W(size), m_H(size) {}

void CheckBox::SetPosition(float x, float y) { m_X=x; m_Y=y; }
void CheckBox::SetSize(float size)           { m_W=size; m_H=size; }
void CheckBox::SetRadius(float r)            { m_Radius = r; }
void CheckBox::SetChecked(bool checked) {
    m_Checked = checked;
    m_AnimT = checked ? 1.0f : 0.0f; // jump if set programmatically before first draw
}
void CheckBox::SetOnChange(std::function<void(bool)> cb) { m_OnChange = cb; }
void CheckBox::SetBoxColor(float r,float g,float b,float a)  { m_BoxR=r; m_BoxG=g; m_BoxB=b; m_BoxA=a; }
void CheckBox::SetFillColor(float r,float g,float b,float a) { m_FillR=r; m_FillG=g; m_FillB=b; m_FillA=a; }

bool CheckBox::HitTest(float x, float y) const {
    float gap = m_Label.empty() ? 0.0f : 8.0f;
    float totalW = m_W + gap + m_LabelWidth;
    return x >= m_X && x <= m_X + totalW && y >= m_Y && y <= m_Y + m_H;
}

bool CheckBox::OnTouch(float x, float y) {
    if (HitTest(x, y)) { m_IsDown = true; return true; }
    return false;
}

void CheckBox::OnTouchMove(float x, float y) {
    if (m_IsDown && !HitTest(x, y)) m_IsDown = false;
}

void CheckBox::OnRelease(float x, float y) {
    if (m_IsDown && HitTest(x, y)) {
        m_Checked = !m_Checked;
        if (m_OnChange) m_OnChange(m_Checked);
    }
    m_IsDown = false;
}

void CheckBox::Draw(UIRenderer& ui) {
    auto now = std::chrono::steady_clock::now();
    float dt = m_FirstFrame ? 0.0f : std::chrono::duration<float>(now - m_LastTime).count();
    m_LastTime = now;
    m_FirstFrame = false;

    float r = std::min(m_Radius, std::min(m_W, m_H) * 0.5f);

    // Animate toward target state
    float target = m_Checked ? 1.0f : 0.0f;
    m_AnimT += (target - m_AnimT) * std::min(1.0f, dt * 12.0f);
    if (std::abs(target - m_AnimT) < 0.01f) m_AnimT = target;

    // Outer box with a slight scale "pop" during transition
    float pop = 1.0f + 0.08f * std::sin(m_AnimT * 3.14159265f); // bulges mid-transition
    float w = m_W * pop, h = m_H * pop;
    float bx = m_X - (w - m_W) * 0.5f;
    float by = m_Y - (h - m_H) * 0.5f;

    ui.DrawRect(bx, by, w, h, m_BoxR, m_BoxG, m_BoxB, m_BoxA, r * pop);

    if (m_AnimT > 0.01f) {
        // Fill fades/scales in
        ui.DrawRect(bx, by, w, h, m_FillR, m_FillG, m_FillB, m_FillA * m_AnimT, r * pop);

        // Checkmark scales in around its own center
        float checkScale = m_AnimT;
        float csize = m_W * checkScale;
        float cx = m_X + (m_W - csize) * 0.5f;
        float cy = m_Y + (m_H - csize) * 0.5f;
        float stroke = m_W * 0.12f;
        ui.DrawCheck(cx, cy, csize, stroke, 1.0f, 1.0f, 1.0f, m_AnimT);
    }

    if (!m_Label.empty()) {
        m_LabelWidth = ui.MeasureText(m_Label, m_FontSize);
        float tx = m_X + m_W + 8.0f;
        float ty = m_Y + (m_H - m_FontSize) * 0.5f;
        ui.DrawText(tx, ty, m_Label, m_LabelR, m_LabelG, m_LabelB, m_LabelA, m_FontSize);
    }
}
