#pragma once
#include "BananaSDK/Button.h"
#include "BananaSDK/Box.h"
#include <variant>
#include <memory>

namespace UIEl {
    using UIElement = std::variant<std::shared_ptr<Button>, std::shared_ptr<Box>>;
}
