#include "BananaSDK/InputField.h"
#include "BananaSDK/UI.h"
#include "BananaSDK/AndroidApp.h"
#include <android/input.h>

InputField::InputField(float x, float y, float w, float h)
    : m_X(x), m_Y(y), m_W(w), m_H(h),
      m_Radius(9999.0f),
      m_BgR(0.80f), m_BgG(0.80f), m_BgB(0.82f) {}
      
extern AndroidApp* g_AppInstance;

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
    if (HitTest(x, y)) {
        m_IsDown      = true;
        m_IsDragging  = false;
        m_TouchStartX = x;
        return true;
    }
    return false;
}

void InputField::OnTouchMove(float x, float y) {
    if (!m_IsDown) return;
    float dx = m_TouchStartX - x;
    if (std::abs(dx) > 8.0f) m_IsDragging = true;
    if (m_IsDragging) {
        m_ScrollOffset += dx;
        m_TouchStartX   = x;
        float maxScroll = std::max(0.0f, m_LastTextW - (m_W - 24.0f));
        m_ScrollOffset  = std::max(0.0f, std::min(m_ScrollOffset, maxScroll));
    }
}

void InputField::OnRelease(float x, float y) {
    if (m_IsDown && !m_IsDragging && HitTest(x, y)) _Focus();
    m_IsDown     = false;
    m_IsDragging = false;
}

void InputField::OnFocusLost() { _Unfocus(); }

void InputField::_Focus() {
    m_Focused = true;
    if (!m_Activity) return;
    JNIEnv* env = nullptr;
    bool attached = false;
    if (m_Activity->vm->GetEnv((void**)&env, JNI_VERSION_1_6) == JNI_EDETACHED) {
        m_Activity->vm->AttachCurrentThread(&env, nullptr);
        attached = true;
    }
    jclass cls = env->GetObjectClass(m_Activity->clazz);
    jmethodID mid = env->GetMethodID(cls, "showEditInput", "(Ljava/lang/String;Z)V");
    jstring jtext = env->NewStringUTF(m_Text.c_str());
    env->CallVoidMethod(m_Activity->clazz, mid, jtext, (jboolean)JNI_FALSE);
    env->DeleteLocalRef(jtext);
    env->DeleteLocalRef(cls);
    if (g_AppInstance) g_AppInstance->SetImeFocused(true);
    if (attached) m_Activity->vm->DetachCurrentThread();
}

void InputField::_Unfocus() {
    m_Focused = false;
    if (!m_Activity) return;
    JNIEnv* env = nullptr;
    bool attached = false;
    if (m_Activity->vm->GetEnv((void**)&env, JNI_VERSION_1_6) == JNI_EDETACHED) {
        m_Activity->vm->AttachCurrentThread(&env, nullptr);
        attached = true;
    }
    jclass cls = env->GetObjectClass(m_Activity->clazz);
    jmethodID mid = env->GetMethodID(cls, "hideEditInput", "()V");
    env->CallVoidMethod(m_Activity->clazz, mid);
    env->DeleteLocalRef(cls);
    if (g_AppInstance) g_AppInstance->SetImeFocused(false);
    if (attached) m_Activity->vm->DetachCurrentThread();
}

bool InputField::OnKey(int32_t keyCode, int32_t unicode) {
    if (!m_Focused) return false;

    /*if (keyCode == AKEYCODE_DEL) {
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
    }*/

    return false;
}

void InputField::Draw(UIRenderer& ui) {
    auto now = std::chrono::steady_clock::now();
    float dt = std::chrono::duration<float>(now - m_LastTime).count();
    m_LastTime = now;
    if (m_Focused) { m_CursorBlink += dt; if (m_CursorBlink > 1.0f) m_CursorBlink -= 1.0f; }

    float r   = std::min(m_Radius, std::min(m_W, m_H) / 2.0f);
    float pad = 12.0f;
    float maxTextW = m_W - pad * 2.0f;

    if (m_Focused)
        ui.DrawRect(m_X-2, m_Y-2, m_W+4, m_H+4, 0.0f, 0.478f, 1.0f, 1.0f, r+2);
    ui.DrawRect(m_X, m_Y, m_W, m_H, m_BgR, m_BgG, m_BgB, 1.0f, r);

    // Vertically center accounting for full font metrics height
    float ty = m_Y + (m_H - m_FontSize * 1.2f) * 0.5f;

    ui.PushScissor(m_X + pad, m_Y, maxTextW, m_H);

    if (m_Text.empty() && !m_Placeholder.empty()) {
        ui.DrawText(m_X + pad, ty, m_Placeholder, 0.65f, 0.65f, 0.65f, 1.0f, m_FontSize);
    } else {
        float textW = ui.MeasureText(m_Text, m_FontSize);
        m_LastTextW = textW;

        // Auto-scroll to keep cursor (end of text) visible when focused
        float maxScroll = std::max(0.0f, textW - maxTextW);
        if (m_Text.size() != m_PrevTextLen) {
            m_ScrollOffset = maxScroll;
            m_PrevTextLen  = m_Text.size();
        }
        m_ScrollOffset = std::max(0.0f, std::min(m_ScrollOffset, maxScroll));

        float tx = m_X + pad - m_ScrollOffset;
        ui.DrawText(tx, ty, m_Text, 0.1f, 0.1f, 0.1f, 1.0f, m_FontSize);

        // Cursor always at end, auto-scroll ensures it's always visible
        if (m_Focused && m_CursorBlink < 0.5f) {
            float cx = tx + textW;
            ui.DrawRect(cx + 2, ty, 2.0f, m_FontSize, 0.0f, 0.478f, 1.0f, 1.0f);
        }
    }

    ui.PopScissor();
}

void InputField::SetTextFromIME(const std::string& text) {
    m_Text = text;
    if (m_OnChange) m_OnChange(m_Text);
}

void InputField::TriggerSubmit(const std::string& text) {
    if (m_OnSubmit) m_OnSubmit(text);
}
