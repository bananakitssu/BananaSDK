#pragma once
#include "PopupMenu.h"

// Like PopupMenu but anchored to a specific screen position
class ContextMenu : public PopupMenu {
public:
    void ShowAt(float anchorX, float anchorY, UIRenderer& ui);
};
