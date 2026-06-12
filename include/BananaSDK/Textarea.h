#pragma once
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <android/native_activity.h>

class UIRenderer;

class Textarea {
public:
    Textarea() = default;
    Textarea(float x, float y, float w, float h);

    void SetActivity(ANativeActivity* activity);
    void SetPlaceholder(const std::string& text);
    void SetText(const std::string& text);
    void SetFontSize(float size);
    void SetRadius(float radius);
    void SetLineSpacing(float spacing);
    void SetOnChange(std::function<void(const std::string&)> cb);
    void SetOnSubmit(std::function<void(const std::string&)> cb);

    std::string GetText() const;
    bool IsFocused() const;

    bool HitTest(float x, float y) const;
    bool OnTouch(float x, float y);
    void OnTouchMove(float x, float y);
    void OnRelease(float x, float y);
    bool OnKey(int32_t keyCode, int32_t unicode);
    void OnFocusLost();

    void Draw(UIRenderer& ui);

    void SetTextFromIME(const std::string& text);
    void TriggerSubmit(const std::string& text);

private:
    void _Focus();
    void _Unfocus();
    std::vector<std::string> _GetLines(UIRenderer& ui);

    float m_X=0, m_Y=0, m_W=0, m_H=0;
    float m_FontSize     = 32.0f;
    float m_Radius       = 24.0f;
    float m_LineSpacing  = 4.0f;
    float m_BgR=0.80f, m_BgG=0.80f, m_BgB=0.82f;
    std::string m_Text;
    std::string m_Placeholder;
    bool  m_Focused      = false;
    bool  m_IsDown       = false;
    bool  m_IsDragging   = false;
    float m_ScrollOffset = 0.0f;
    float m_LastContentH = 0.0f;
    float m_TouchStartY  = 0.0f;
    float m_CursorBlink  = 0.0f;
    float m_ScrollOffsetX = 0.0f;
    float m_TouchStartX   = 0.0f;
    float m_LastMaxScrollX = 0.0f;
    size_t m_PrevTextLen = 0;
    std::vector<std::string> m_LinesCache;
    bool m_LinesDirty = true;
    std::chrono::steady_clock::time_point m_LastTime;
    ANativeActivity* m_Activity = nullptr;
    std::function<void(const std::string&)> m_OnChange;
    std::function<void(const std::string&)> m_OnSubmit;
};
