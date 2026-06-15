#include "BananaSDK/ScrollBox.h"
#include "BananaSDK/UI.h"
#include <algorithm>
#include <cmath>

ScrollBox::ScrollBox(float x, float y, float w, float h)
    : m_X(x), m_Y(y), m_W(w), m_H(h), m_ScrollOffset(0.0f) {}

void ScrollBox::SetPosition(float x, float y) { m_X=x; m_Y=y; }
void ScrollBox::SetSize(float w, float h)     { m_W=w; m_H=h; }
void ScrollBox::SetRadius(float r)            { m_Radius = r; }
void ScrollBox::SetPadding(float p)           { m_Padding = p; }
void ScrollBox::SetContentHeight(float h)     { m_ContentH = h; }
void ScrollBox::SetBgColor(float r,float g,float b,float a) { m_BgR=r; m_BgG=g; m_BgB=b; m_BgA=a; }
void ScrollBox::SetScrollbarColor(float r,float g,float b,float a) { m_SbR=r; m_SbG=g; m_SbB=b; m_SbA=a; }

void ScrollBox::AddElement(UIEl::UIElement el) {
    m_Elements.push_back(el);
}

float ScrollBox::_MaxScroll() const {
    return std::max(0.0f, (m_ContentH + m_Padding) - m_H);
}

const float SCROLLBAR_WIDTH = 8.0f;
const float SCROLLBAR_MARGIN = 4.0f;

void ScrollBox::_GetThumbRect(float& tx, float& ty, float& tw, float& th) const {
    float maxScroll = _MaxScroll();
    tw = SCROLLBAR_WIDTH;
    tx = m_X + m_W - SCROLLBAR_WIDTH - SCROLLBAR_MARGIN;

    if (m_ContentH <= m_H || m_ContentH <= 0.0f) {
        th = m_H;
        ty = m_Y;
        return;
    }

    th = std::max(20.0f, m_H * (m_H / m_ContentH));
    float trackH = m_H - th;
    float t = (maxScroll > 0.0f) ? (m_ScrollOffset / maxScroll) : 0.0f;
    ty = m_Y + trackH * t;
}

bool ScrollBox::HitTest(float x, float y) const {
    return x >= m_X && x <= m_X + m_W && y >= m_Y && y <= m_Y + m_H;
}

bool ScrollBox::OnTouch(float x, float y) {
    if (!HitTest(x, y)) return false;

    float tx, ty, tw, th;
    _GetThumbRect(tx, ty, tw, th);
    bool onThumb = (x >= tx && x <= tx + tw && y >= ty && y <= ty + th) && (m_ContentH > m_H);

    m_IsDown        = true;
    m_IsDragging    = false;
    m_DraggingThumb = onThumb;
    m_TouchStartY   = y;
    m_ScrollStart   = m_ScrollOffset;

    if (!onThumb) {
        float localX = x - m_X;
        float localY = y - m_Y + m_ScrollOffset;
        for (int i = (int)m_Elements.size() - 1; i >= 0; i--) {
            bool hit = std::visit([localX, localY](auto& ptr) -> bool {
                return ptr->HitTest(localX, localY);
            }, m_Elements[i]);
            if (hit) {
                std::visit([localX, localY](auto& ptr) { ptr->OnTouch(localX, localY); }, m_Elements[i]);
                break;
            }
        }
    }
    return true;
}

void ScrollBox::OnTouchMove(float x, float y) {
    if (!m_IsDown) return;

    float dy = m_TouchStartY - y;

    if (m_DraggingThumb) {
        float maxScroll = _MaxScroll();
        float trackH = m_H - std::max(20.0f, m_H * (m_H / std::max(m_ContentH, m_H)));
        if (trackH > 0.0f) {
            float scrollDelta = (dy * -1.0f) * (maxScroll / trackH);
            m_ScrollOffset = std::clamp(m_ScrollStart - scrollDelta, 0.0f, maxScroll);
        }
        return;
    }

    if (std::abs(dy) > 8.0f) m_IsDragging = true;

    if (m_IsDragging) {
        float maxScroll = _MaxScroll();
        m_ScrollOffset = std::clamp(m_ScrollStart + dy, 0.0f, maxScroll);
    } else {
        float localX = x - m_X;
        float localY = y - m_Y + m_ScrollOffset;
        for (auto& el : m_Elements)
            std::visit([localX, localY](auto& ptr) { ptr->OnTouchMove(localX, localY); }, el);
    }
}

void ScrollBox::OnRelease(float x, float y) {
    if (!m_IsDown) return;

    if (!m_DraggingThumb && !m_IsDragging) {
        float localX = x - m_X;
        float localY = y - m_Y + m_ScrollOffset;
        for (auto& el : m_Elements)
            std::visit([localX, localY](auto& ptr) { ptr->OnRelease(localX, localY); }, el);
    }

    m_IsDown        = false;
    m_IsDragging    = false;
    m_DraggingThumb = false;
}

void ScrollBox::Draw(UIRenderer& ui) {
    ui.DrawRect(m_X, m_Y, m_W, m_H, m_BgR, m_BgG, m_BgB, m_BgA, m_Radius);

    ui.PushRoundedScissor(m_X, m_Y, m_W, m_H, m_Radius);

    // Draw children offset by scroll
    for (auto& el : m_Elements) {
        std::visit([&ui, this](auto& ptr) {
            float ox = ptr->GetX();
            float oy = ptr->GetY();  // this is the LOCAL/relative position (0-based)

            ptr->SetPosition(m_X + ox + m_Padding, m_Y + oy - m_ScrollOffset);
            ptr->Draw(ui);
            ptr->SetPosition(ox, oy); // restore to local coords
        }, el);
    }

    // Scrollbar thumb
    if (m_ContentH > m_H) {
        float tx, ty, tw, th;
        _GetThumbRect(tx, ty, tw, th);
        ui.DrawRect(tx, ty, tw, th, m_SbR, m_SbG, m_SbB, m_SbA, tw * 0.5f);
    }

    ui.PopRoundedScissor();
}
