#include "BananaSDK/CheckBox.h"
#include "BananaSDK/UI.h"
#include <algorithm>

CheckBox::CheckBox(float x, float y, float size)
    : m_X(x), m_Y(y), m_W(size), m_H(size) {}

void CheckBox::SetPosition(float x, float y) { m_X=x; m_Y=y; }
void CheckBox::SetSize(float size)           { m_W=size; m_H=size; }
void CheckBox::SetRadius(float r)            { m_Radius = r; }
void CheckBox::SetChecked(bool checked)      { m_Checked = checked; }
void CheckBox::SetOnChange(std::function<void(bool)> cb) { m_OnChange = cb; }
void CheckBox::SetBoxColor(float r,float g,float b,float a)  { m_BoxR=r; m_BoxG=g; m_BoxB=b; m_BoxA=a; }
void CheckBox::SetFillColor(float r,float g,float b,float a) { m_FillR=r; m_FillG=g; m_FillB=b; m_FillA=a; }

bool CheckBox::HitTest(float x, float y) const {
    return x >= m_X && x <= m_X + m_W && y >= m_Y && y <= m_Y + m_H;
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
    float r = std::min(m_Radius, std::min(m_W, m_H) * 0.5f);

    // Outer box
    ui.DrawRect(m_X, m_Y, m_W, m_H, m_BoxR, m_BoxG, m_BoxB, m_BoxA, r);

    if (m_Checked) {
        // Filled accent square, inset slightly
        float inset = m_W * 0.18f;
        float ix = m_X + inset;
        float iy = m_Y + inset;
        float iw = m_W - inset * 2.0f;
        float ih = m_H - inset * 2.0f;
        float ir = std::max(0.0f, r - inset);
        ui.DrawRect(ix, iy, iw, ih, m_FillR, m_FillG, m_FillB, m_FillA, ir);
    }
}
