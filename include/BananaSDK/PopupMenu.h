#pragma once
#include "UI.h"
#include "Button.h"
#include <string>
#include <vector>
#include <functional>

class PopupMenu {
public:
    PopupMenu() = default;

    void AddItem(const std::string& label, std::function<void()> cb);
    void AddLabel(const std::string& text); // non-clickable text row
    void Show(UIRenderer& ui);
    void Hide();
    bool IsVisible() const { return m_Visible; }

    void Draw(UIRenderer& ui);
    bool OnTouch(float x, float y);
    void OnRelease(float x, float y);

protected:
    struct Item {
        std::string label;
        std::function<void()> onClick;
        Button btn;
        bool isLabel = false;
    };
    std::vector<Item> m_Items;
    bool  m_Visible = false;
    float m_CardX=0, m_CardY=0, m_CardW=0, m_CardH=0;
    float m_SW=0, m_SH=0;
};
