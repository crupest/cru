#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "pre.hpp"

#include <map>
#include <initializer_list>

#include "ui/control.hpp"

namespace cru::ui::controls
{
    class ListItem : public ContentControl
    {
    public:
        static constexpr auto control_type = L"ListItem";

        enum class State
        {
            Normal,
            Hover,
            Select
        };

    private:
        struct StateBrush
        {
            Microsoft::WRL::ComPtr<ID2D1Brush> border_brush;
            Microsoft::WRL::ComPtr<ID2D1Brush> fill_brush;
        };

    public:
        static ListItem* Create(Control* child = nullptr)
        {
            const auto list_item = new ListItem();
            list_item->SetChild(child);
            return list_item;
        }

    private:
        ListItem();
    public:
        ListItem(const ListItem& other) = delete;
        ListItem(ListItem&& other) = delete;
        ListItem& operator=(const ListItem& other) = delete;
        ListItem& operator=(ListItem&& other) = delete;
        ~ListItem() override = default;

        StringView GetControlType() const override;

        State GetState() const
        {
            return state_;
        }

        void SetState(State state);

    private:
        State state_ = State::Normal;
        std::map<State, StateBrush> brushes_{};
    };
}
