#pragma once
#include "BananaSDK/UIElements.h"
#include <vector>

class UIRenderer;

class ScrollBox {
public:
    ScrollBox() = default;
    ScrollBox(float x, float y, float w, float h);

    void SetPosition(float x, float y);
    void SetSize(float w, float h);
    void SetRadius(float radius);
    void SetPadding(float padding);
    void SetBgColor(float r, float g, float b, float a = 1.0f);
    void SetScrollbarColor(float r, float g, float b, float a = 1.0f);
    void SetContentHeight(float h);  // total scrollable content height

    void AddElement(UIEl::UIElement el);

    float GetX() const { return m_X; }
    float GetY() const { return m_Y; }
    float GetW() const { return m_W; }
    float GetH() const { return m_H; }
    float GetScrollOffset() const { return m_ScrollOffset; }

    bool HitTest(float x, float y) const;
    bool OnTouch(float x, float y);
    void OnTouchMove(float x, float y);
    void OnRelease(float x, float y);

    void Draw(UIRenderer& ui);

private:
    float m_X=0, m_Y=0, m_W=0, m_H=0;
    float m_Radius     = 12.0f;
    float m_ContentH   = 0.0f;
    float m_ScrollOffset = 0.0f;

    float m_BgR=1.0f, m_BgG=1.0f, m_BgB=1.0f, m_BgA=1.0f;
    float m_SbR=0.6f, m_SbG=0.6f, m_SbB=0.6f, m_SbA=1.0f;

    bool  m_IsDown     = false;
    bool  m_IsDragging = false;
    bool  m_DraggingThumb = false;
    float m_TouchStartY = 0.0f;
    float m_ScrollStart = 0.0f;

    float m_Padding = 12.0f;

    std::vector<UIEl::UIElement> m_Elements;

    float _MaxScroll() const;
    void  _GetThumbRect(float& tx, float& ty, float& tw, float& th) const;
};
