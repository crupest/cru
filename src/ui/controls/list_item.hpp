#pragma once

#include "ui/control.hpp"

namespace cru::ui::controls
{
    class ListItem : public Control
    {
    public:
        static constexpr auto control_type = L"ListItem";

    public:
        static ListItem* Create();
    private:
        ListItem();
    public:
        ListItem(const ListItem& other) = delete;
        ListItem(ListItem&& other) = delete;
        ListItem& operator=(const ListItem& other) = delete;
        ListItem& operator=(ListItem&& other) = delete;
        ~ListItem() override = default;
    };
}
