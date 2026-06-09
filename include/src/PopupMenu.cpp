#include "BananaSDK/PopupMenu.h"

void PopupMenu::AddItem(const std::string& label, std::function<void()> cb) {
    Item item;
    item.label   = label;
    item.onClick = cb;
    item.isLabel = false;
    m_Items.push_back(std::move(item));
}

void PopupMenu::AddLabel(const std::string& text) {
    Item item;
    item.label   = text;
    item.isLabel = true;
    m_Items.push_back(std::move(item));
}

void PopupMenu::Show(UIRenderer& ui) {
    m_SW = (float)ui.GetWidth();
    m_SH = (float)ui.GetHeight();

    float rowH   = 60.0f;
    float pad    = 20.0f;
    m_CardW = m_SW - 80.0f;
    m_CardH = pad + rowH * (float)m_Items.size() + pad;
    m_CardX = (m_SW - m_CardW) / 2.0f;
    m_CardY = (m_SH - m_CardH) / 2.0f;

    for (size_t i = 0; i < m_Items.size(); i++) {
        if (!m_Items[i].isLabel) {
            float btnY = m_CardY + pad + (float)i * rowH;
            m_Items[i].btn = Button(m_CardX + pad, btnY,
                                     m_CardW - pad * 2, rowH - 8.0f,
                                     m_Items[i].label);
            m_Items[i].btn.SetOnClick(m_Items[i].onClick);
        }
    }
    m_Visible = true;
}

void PopupMenu::Hide() { m_Visible = false; }

void PopupMenu::Draw(UIRenderer& ui) {
    if (!m_Visible) return;

    // Scrim
    ui.DrawRect(0, 0, m_SW, m_SH, 0.0f, 0.0f, 0.0f, 0.45f);

    // Card
    ui.DrawRect(m_CardX, m_CardY, m_CardW, m_CardH, 1.0f, 1.0f, 1.0f, 1.0f, 24.0f);

    float pad  = 20.0f;
    float rowH = 60.0f;

    for (size_t i = 0; i < m_Items.size(); i++) {
        float rowY = m_CardY + pad + (float)i * rowH;
        if (m_Items[i].isLabel) {
            // Draw as grey centered-ish text
            ui.DrawText(m_CardX + pad, rowY + 12.0f,
                        m_Items[i].label, 0.4f, 0.4f, 0.4f, 1.0f, 24.0f);
        } else {
            m_Items[i].btn.Draw(ui);
        }
    }
}

bool PopupMenu::OnTouch(float x, float y) {
    if (!m_Visible) return false;
    for (auto& item : m_Items) {
        if (!item.isLabel && item.btn.OnTouch(x, y)) return true;
    }
    // Tap outside card = dismiss
    if (x < m_CardX || x > m_CardX + m_CardW ||
        y < m_CardY || y > m_CardY + m_CardH) {
        Hide();
        return true;
    }
    return false;
}

void PopupMenu::OnRelease() {
    if (!m_Visible) return;
    for (auto& item : m_Items)
        if (!item.isLabel) item.btn.OnRelease();
}
