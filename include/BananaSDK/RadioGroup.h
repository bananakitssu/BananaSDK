#pragma once
#include <string>
#include <vector>
#include <functional>
#include <chrono>

class UIRenderer;

class RadioGroup {
public:
    RadioGroup() = default;
    RadioGroup(float x, float y, float w, float h);

    void SetPosition(float x, float y);
    void SetSize(float w, float h);
    void SetRadius(float radius);
    void SetPadding(float padding);
    void SetRowHeight(float h);
    void SetBgColor(float r, float g, float b, float a = 1.0f);
    void SetScrollbarColor(float r, float g, float b, float a = 1.0f);
    void SetRadioColor(float r, float g, float b, float a = 1.0f);
    void SetTextColor(float r, float g, float b, float a = 1.0f);
    void SetFontSize(float size);
    void SetOnChange(std::function<void(int)> cb);

    void AddOption(const std::string& label);
    int  GetSelected() const { return m_Selected; }
    void SetSelected(int index);

    float GetX() const { return m_X; }
    float GetY() const { return m_Y; }
    float GetW() const { return m_W; }
    float GetH() const { return m_H; }

    bool HitTest(float x, float y) const;
    bool OnTouch(float x, float y);
    void OnTouchMove(float x, float y);
    void OnRelease(float x, float y);

    void Draw(UIRenderer& ui);

private:
    float _ContentH() const;
    float _MaxScroll() const;
    void  _GetThumbRect(float& tx, float& ty, float& tw, float& th) const;

    float m_X=0, m_Y=0, m_W=0, m_H=0;
    float m_Radius    = 12.0f;
    float m_Padding   = 12.0f;
    float m_RowHeight = 48.0f;
    float m_FontSize  = 26.0f;

    float m_BgR=1.0f, m_BgG=1.0f, m_BgB=1.0f, m_BgA=1.0f;
    float m_SbR=0.6f, m_SbG=0.6f, m_SbB=0.6f, m_SbA=1.0f;
    float m_RadioR=0.0f, m_RadioG=0.478f, m_RadioB=1.0f, m_RadioA=1.0f;
    float m_TextR=0.1f, m_TextG=0.1f, m_TextB=0.1f, m_TextA=1.0f;

    std::vector<std::string> m_Options;
    int m_Selected = -1;

    float m_ScrollOffset = 0.0f;
    bool  m_IsDown=false, m_IsDragging=false, m_DraggingThumb=false;
    float m_TouchStartY=0.0f, m_ScrollStart=0.0f;
    float m_TouchStartX_=0.0f, m_TouchY_=0.0f;

    std::vector<float> m_AnimT;
    std::chrono::steady_clock::time_point m_LastTime;
    bool m_FirstFrame = true;

    std::function<void(int)> m_OnChange;
};
