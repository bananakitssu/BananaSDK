#include "BananaSDK/Textarea.h"
#include "BananaSDK/UI.h"
#include <android/input.h>
#include <jni.h>
#include <sstream>
#include <algorithm>
#include <cmath>

Textarea::Textarea(float x, float y, float w, float h)
    : m_X(x), m_Y(y), m_W(w), m_H(h) {}

void Textarea::SetActivity(ANativeActivity* a)                         { m_Activity = a; }
void Textarea::SetPlaceholder(const std::string& t)                    { m_Placeholder = t; }
void Textarea::SetText(const std::string& t)                           { m_Text = t; m_LinesDirty = true; }
void Textarea::SetFontSize(float s)                                    { m_FontSize = s; }
void Textarea::SetRadius(float r)                                      { m_Radius = r; }
void Textarea::SetLineSpacing(float s)                                 { m_LineSpacing = s; }
void Textarea::SetOnChange(std::function<void(const std::string&)> cb) { m_OnChange = cb; }
void Textarea::SetOnSubmit(std::function<void(const std::string&)> cb) { m_OnSubmit = cb; }
std::string Textarea::GetText() const { return m_Text; }
bool Textarea::IsFocused() const      { return m_Focused; }

bool Textarea::HitTest(float x, float y) const {
    return x >= m_X && x <= m_X + m_W && y >= m_Y && y <= m_Y + m_H;
}

bool Textarea::OnTouch(float x, float y) {
    if (HitTest(x, y)) {
        m_IsDown      = true;
        m_IsDragging  = false;
        m_TouchStartX = x;
        m_TouchStartY = y;
        return true;
    }
    return false;
}

void Textarea::OnTouchMove(float x, float y) {
    if (!m_IsDown) return;
    if (!HitTest(x, y)) { m_IsDown = false; m_IsDragging = false; return; }

    float dx = m_TouchStartX - x;
    float dy = m_TouchStartY - y;
    if (std::abs(dx) > 8.0f || std::abs(dy) > 8.0f) m_IsDragging = true;

    if (m_IsDragging) {
        m_ScrollOffset  += dy;
        m_ScrollOffsetX += dx;
        m_TouchStartX    = x;
        m_TouchStartY    = y;

        float maxScroll = std::max(0.0f, m_LastContentH - m_H);
        m_ScrollOffset  = std::max(0.0f, std::min(m_ScrollOffset, maxScroll));
        m_ScrollOffsetX = std::max(0.0f, std::min(m_ScrollOffsetX, m_LastMaxScrollX));
    }
}

void Textarea::OnRelease(float x, float y) {
    if (m_IsDown && !m_IsDragging && HitTest(x, y)) _Focus();
    m_IsDown     = false;
    m_IsDragging = false;
}

void Textarea::OnFocusLost() { _Unfocus(); }

void Textarea::_Focus() {
    m_Focused = true;
    if (!m_Activity) return;
    JNIEnv* env = nullptr;
    m_Activity->vm->AttachCurrentThread(&env, nullptr);
    jclass  actClass  = env->GetObjectClass(m_Activity->clazz);
    jobject window    = env->CallObjectMethod(m_Activity->clazz, env->GetMethodID(actClass, "getWindow", "()Landroid/view/Window;"));
    jobject decorView = env->CallObjectMethod(window, env->GetMethodID(env->GetObjectClass(window), "getDecorView", "()Landroid/view/View;"));
    jstring svc       = env->NewStringUTF("input_method");
    jobject imm       = env->CallObjectMethod(m_Activity->clazz, env->GetMethodID(actClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;"), svc);
    env->CallBooleanMethod(imm, env->GetMethodID(env->GetObjectClass(imm), "showSoftInput", "(Landroid/view/View;I)Z"), decorView, (jint)0);
    env->DeleteLocalRef(svc); env->DeleteLocalRef(imm);
    env->DeleteLocalRef(decorView); env->DeleteLocalRef(window); env->DeleteLocalRef(actClass);
    m_Activity->vm->DetachCurrentThread();
}

void Textarea::_Unfocus() {
    m_Focused = false;
    if (!m_Activity) return;
    JNIEnv* env = nullptr;
    m_Activity->vm->AttachCurrentThread(&env, nullptr);
    jclass  actClass  = env->GetObjectClass(m_Activity->clazz);
    jobject window    = env->CallObjectMethod(m_Activity->clazz, env->GetMethodID(actClass, "getWindow", "()Landroid/view/Window;"));
    jobject decorView = env->CallObjectMethod(window, env->GetMethodID(env->GetObjectClass(window), "getDecorView", "()Landroid/view/View;"));
    jstring svc       = env->NewStringUTF("input_method");
    jobject imm       = env->CallObjectMethod(m_Activity->clazz, env->GetMethodID(actClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;"), svc);
    jobject binder    = env->CallObjectMethod(decorView, env->GetMethodID(env->GetObjectClass(decorView), "getWindowToken", "()Landroid/os/IBinder;"));
    env->CallBooleanMethod(imm, env->GetMethodID(env->GetObjectClass(imm), "hideSoftInputFromWindow", "(Landroid/os/IBinder;I)Z"), binder, (jint)0);
    env->DeleteLocalRef(binder); env->DeleteLocalRef(svc); env->DeleteLocalRef(imm);
    env->DeleteLocalRef(decorView); env->DeleteLocalRef(window); env->DeleteLocalRef(actClass);
    m_Activity->vm->DetachCurrentThread();
}

bool Textarea::OnKey(int32_t keyCode, int32_t unicode) {
    if (!m_Focused) return false;

    if (keyCode == AKEYCODE_DEL) {
        if (!m_Text.empty()) {
            int i = (int)m_Text.size() - 1;
            while (i > 0 && (m_Text[i] & 0xC0) == 0x80) i--;
            m_Text.erase(i);
            if (m_OnChange) m_OnChange(m_Text);
        }
        m_LinesDirty = true;
        return true;
    }

    if (keyCode == AKEYCODE_ENTER || keyCode == AKEYCODE_NUMPAD_ENTER) {
        m_Text += '\n';
        if (m_OnChange) m_OnChange(m_Text);
        m_LinesDirty = true;
        return true;
    }

    if (unicode > 0) {
        if      (unicode < 0x80)  { m_Text += (char)unicode; }
        else if (unicode < 0x800) { m_Text += (char)(0xC0|(unicode>>6)); m_Text += (char)(0x80|(unicode&0x3F)); }
        else                      { m_Text += (char)(0xE0|(unicode>>12)); m_Text += (char)(0x80|((unicode>>6)&0x3F)); m_Text += (char)(0x80|(unicode&0x3F)); }
        if (m_OnChange) m_OnChange(m_Text);
        m_LinesDirty = true;
        return true;
    }

    return false;
}

std::vector<std::string> Textarea::_GetLines(UIRenderer& ui) {
    if (!m_LinesDirty) return m_LinesCache;

    m_LinesCache.clear();
    float maxW = m_W - 24.0f;

    std::vector<std::string> paragraphs;
    std::string para;
    for (char c : m_Text) {
        if (c == '\n') { paragraphs.push_back(para); para.clear(); }
        else para += c;
    }
    paragraphs.push_back(para);

    for (auto& p : paragraphs) {
        if (p.empty()) { m_LinesCache.push_back(""); continue; }
        std::istringstream stream(p);
        std::string word, currentLine;
        while (stream >> word) {
            std::string candidate = currentLine.empty() ? word : currentLine + " " + word;
            if (ui.MeasureText(candidate, m_FontSize) > maxW && !currentLine.empty()) {
                m_LinesCache.push_back(currentLine);
                currentLine = word;
            } else {
                currentLine = candidate;
            }
        }
        if (!currentLine.empty()) m_LinesCache.push_back(currentLine);
    }

    if (m_LinesCache.empty()) m_LinesCache.push_back("");
    m_LinesDirty = false;
    return m_LinesCache;
}

void Textarea::Draw(UIRenderer& ui) {
    auto now = std::chrono::steady_clock::now();
    float dt = std::chrono::duration<float>(now - m_LastTime).count();
    m_LastTime = now;
    if (m_Focused) { m_CursorBlink += dt; if (m_CursorBlink > 1.0f) m_CursorBlink -= 1.0f; }

    float r   = std::min(m_Radius, std::min(m_W, m_H) / 2.0f);
    float pad = 12.0f;
    float lineH = m_FontSize + m_LineSpacing;

    if (m_Focused)
        ui.DrawRect(m_X-2, m_Y-2, m_W+4, m_H+4, 0.0f, 0.478f, 1.0f, 1.0f, r+2);
    ui.DrawRect(m_X, m_Y, m_W, m_H, m_BgR, m_BgG, m_BgB, 1.0f, r);
    
    ui.PushScissor(m_X, m_Y, m_W, m_H);

    if (m_Text.empty() && !m_Placeholder.empty()) {
        ui.DrawText(m_X + pad, m_Y + pad, m_Placeholder, 0.65f, 0.65f, 0.65f, 1.0f, m_FontSize);
    } else {
        auto lines = _GetLines(ui);

        float contentH  = (float)lines.size() * lineH + pad * 2.0f;
        m_LastContentH  = contentH;
        float maxScroll = std::max(0.0f, contentH - m_H);

        if (m_TextChanged) {
            float lastLineBottom = pad + (float)lines.size() * lineH;
            m_ScrollOffset = std::max(0.0f, lastLineBottom - m_H + pad);
            m_TextChanged  = false;
        }
        m_ScrollOffset = std::max(0.0f, std::min(m_ScrollOffset, maxScroll));

        float maxLineW = 0.0f;
        for (auto& line : lines)
            maxLineW = std::max(maxLineW, ui.MeasureText(line, m_FontSize));
        m_LastMaxScrollX = std::max(0.0f, maxLineW - (m_W - pad * 2.0f));
        m_ScrollOffsetX  = std::max(0.0f, std::min(m_ScrollOffsetX, m_LastMaxScrollX));

        for (size_t i = 0; i < lines.size(); i++) {
            float ly = m_Y + pad + (float)i * lineH - m_ScrollOffset;
            if (ly + m_FontSize < m_Y) continue;
            if (ly > m_Y + m_H)       break;
            ui.DrawText(m_X + pad - m_ScrollOffsetX, ly, lines[i], 0.1f, 0.1f, 0.1f, 1.0f, m_FontSize);
        }

        if (m_Focused && m_CursorBlink < 0.5f && !lines.empty()) {
            size_t last = lines.size() - 1;
            float cy = m_Y + pad + (float)last * lineH - m_ScrollOffset;
            float cx = m_X + pad + ui.MeasureText(lines[last], m_FontSize) - m_ScrollOffsetX;
            if (cy >= m_Y && cy + m_FontSize <= m_Y + m_H)
                ui.DrawRect(cx + 2, cy, 2.0f, m_FontSize, 0.0f, 0.478f, 1.0f, 1.0f);
        }
    }
    
    ui.PopScissor();
}
