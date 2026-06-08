#pragma once
#include "BananaSDK/Button.h"
#include "BananaSDK/Box.h"
#include <variant>

namespace BananaSDK {
    using UIElement = std::variant<Button, Box>;
}
