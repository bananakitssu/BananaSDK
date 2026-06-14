#pragma once
#include "BananaSDK/Button.h"
#include "BananaSDK/Box.h"
#include "BananaSDK/Text.h"
#include "BananaSDK/InputField.h"
#include "BananaSDK/Textarea.h"
#include "BananaSDK/Progress.h"
#include "BananaSDK/CircleProgress.h"
#include <variant>
#include <memory>

namespace UIEl {
    class ScrollBoxFwd;

    using UIElement = std::variant<
        std::shared_ptr<Button>,
        std::shared_ptr<Box>,
        std::shared_ptr<Text>,
        std::shared_ptr<InputField>,
        std::shared_ptr<Textarea>,
        std::shared_ptr<Progress>,
        std::shared_ptr<CircleProgress>
    >;
}

#include "BananaSDK/ScrollBox.h"
