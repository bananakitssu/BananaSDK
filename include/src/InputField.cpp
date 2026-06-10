#include "BananaSDK/InputField.h"
#include "BananaSDK/UI.h"
#include <android/input.h>

InputField::InputField(float x, float y, float w, float h)
    : m_X(x), m_Y(y), m_W(w), m_H(h) {}

void InputField::SetActivity(ANativeActivity* a)                         { m_Activity = a; }
void InputField::SetPlaceholder(const std::string& t)                    { m_Placeholder = t; }
void InputField::SetText(const std::string& t)                           { m_Text = t; }
void InputField::SetFontSize(float s)                                    { m_FontSize = s; }
void InputField::SetRadius(float r)                                      { m_Radius = r; }
void InputField::SetOnChange(std::function<void(const std::string&)> cb) { m_OnChange = cb; }
void InputField::SetOnSubmit(std::function<void(const std::string&)> cb) { m_OnSubmit = cb; }
std::string InputField::GetText() const { return m_Text; }
bool InputField::IsFocused() const      { return m_Focused; }

bool InputField::HitTest(float x, float y) const {
    return x >= m_X && x <= m_X + m_W && y >= m_Y && y <= m_Y + m_H;
}

bool InputField::OnTouch(float x, float y) {
    if (HitTest(x, y)) { m_IsDown = true; return true; }
    return false;
}

void InputField::OnTouchMove(float x, float y) {
    if (m_IsDown && !HitTest(x, y)) m_IsDown = false;
}

void InputField::OnRelease(float x, float y) {
    if (m_IsDown && HitTest(x, y)) _Focus();
    m_IsDown = false;
}

void InputField::OnFocusLost() { _Unfocus(); }

void InputField::_Focus() {
    m_Focused = true;
    if (m_Activity)
        ANativeActivity_showSoftInput(m_Activity, ANATIVEACTIVITY_SHOW_SOFT_INPUT_IMPLICIT);
}

void InputField::_Unfocus() {
    m_Focused = false;
    if (m_Activity)
        ANativeActivity_hideSoftInput(m_Activity, ANATIVEACTIVITY_HIDE_SOFT_INPUT_NOT_ALWAYS);
}

bool InputField::OnKey(int32_t keyCode, int32_t unicode) {
    if (!m_Focused) return false;

    if (keyCode == AKEYCODE_DEL) {
        if (!m_Text.empty()) {
            int i = (int)m_Text.size() - 1;
            while (i > 0 && (m_Text[i] & 0xC0) == 0x80) i--;
            m_Text.erase(i);
            if (m_OnChange) m_OnChange(m_Text);
        }
        return true;
    }

    if (keyCode == AKEYCODE_ENTER || keyCode == AKEYCODE_NUMPAD_ENTER) {
        if (m_OnSubmit) m_OnSubmit(m_Text);
        return true;
    }

    if (unicode > 0) {
        if      (unicode < 0x80)  { m_Text += (char)unicode; }
        else if (unicode < 0x800) { m_Text += (char)(0xC0|(unicode>>6)); m_Text += (char)(0x80|(unicode&0x3F)); }
        else                      { m_Text += (char)(0xE0|(unicode>>12)); m_Text += (char)(0x80|((unicode>>6)&0x3F)); m_Text += (char)(0x80|(unicode&0x3F)); }
        if (m_OnChange) m_OnChange(m_Text);
        return true;
    }

    return false;
}

void InputField::Draw(UIRenderer& ui) {
    auto now = std::chrono::steady_clock::now();
    float dt = std::chrono::duration<float>(now - m_LastTime).count();
    m_LastTime = now;
    if (m_Focused) { m_CursorBlink += dt; if (m_CursorBlink > 1.0f) m_CursorBlink -= 1.0f; }

    if (m_Focused)
        ui.DrawRect(m_X-2, m_Y-2, m_W+4, m_H+4, 0.0f, 0.478f, 1.0f, 1.0f, m_Radius+2);

    ui.DrawRect(m_X, m_Y, m_W, m_H, 1.0f, 1.0f, 1.0f, 1.0f, m_Radius);

    float ty = m_Y + (m_H - m_FontSize) * 0.5f;
    if (m_Text.empty() && !m_Placeholder.empty())
        ui.DrawText(m_X+12, ty, m_Placeholder, 0.65f, 0.65f, 0.65f, 1.0f, m_FontSize);
    else
        ui.DrawText(m_X+12, ty, m_Text, 0.1f, 0.1f, 0.1f, 1.0f, m_FontSize);

    if (m_Focused && m_CursorBlink < 0.5f) {
        float cx = m_X + 12.0f + ui.MeasureText(m_Text, m_FontSize);
        ui.DrawRect(cx, ty, 2.0f, m_FontSize, 0.0f, 0.478f, 1.0f, 1.0f);
    }
}
