#pragma once

#include <vector>
#include <utility>
#include <functional>

#include "base.hpp"
#include "ui/ui_base.hpp"

namespace cru::ui
{
    class Window;
}

namespace cru::ui::controls
{
    using MenuItemInfo = std::pair<String, std::function<void()>>;

    Window* CreatePopupMenu(const Point& anchor, const std::vector<MenuItemInfo>& items, Window* parent = nullptr);
}
