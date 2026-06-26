#pragma once
#include <string>
#include <vector>
#include <functional>
#include <chrono>

class UIRenderer;

class MultiDropdown {
public:
    MultiDropdown() = default;
    MultiDropdown(float x, float y, float w, float h);

    void SetPosition(float x, float y);
    void SetSize(float w, float h);
    void SetRadius(float radius);
    void SetFontSize(float size);
    void SetPlaceholder(const std::string& text);
    void SetBgColor(float r, float g, float b, float a = 1.0f);
    void SetPanelColor(float r, float g, float b, float a = 1.0f);
    void SetTextColor(float r, float g, float b, float a = 1.0f);
    void SetAccentColor(float r, float g, float b, float a = 1.0f);
    void SetOnChange(std::function<void(const std::vector<int>&)> cb);

    void AddOption(const std::string& label);
    std::vector<int> GetSelected() const;
    void SetSelected(int index, bool selected);
    bool IsSelected(int index) const;

    float GetX() const { return m_X; }
    float GetY() const { return m_Y; }
    float GetW() const { return m_W; }
    float GetH() const { return m_H; }

    bool HitTest(float x, float y) const;
    bool OnTouch(float x, float y);
    void OnTouchMove(float x, float y);
    void OnRelease(float x, float y);

    bool IsOpen() const { return m_Open; }
    void Close() { m_Open = false; }

    void Draw(UIRenderer& ui);

private:
    float _PanelHeight() const;
    float _RowHeight() const;
    std::string _SummaryText() const;

    float m_X=0, m_Y=0, m_W=200.0f, m_H=48.0f;
    float m_Radius   = 9999.0f;
    float m_FontSize = 26.0f;
    std::string m_Placeholder = "Select...";

    float m_BgR=0.80f, m_BgG=0.80f, m_BgB=0.82f, m_BgA=1.0f;
    float m_PanelR=1.0f, m_PanelG=1.0f, m_PanelB=1.0f, m_PanelA=1.0f;
    float m_TextR=0.1f, m_TextG=0.1f, m_TextB=0.1f, m_TextA=1.0f;
    float m_AccR=0.0f, m_AccG=0.478f, m_AccB=1.0f, m_AccA=1.0f;

    std::vector<std::string> m_Options;
    std::vector<bool> m_SelectedFlags;
    std::vector<float> m_AnimT; // per-row checkbox animation

    bool m_Open = false;
    bool m_IsDown = false;
    float m_PanelAnimT = 0.0f;
    std::chrono::steady_clock::time_point m_LastTime;
    bool m_FirstFrame = true;

    std::function<void(const std::vector<int>&)> m_OnChange;
};
