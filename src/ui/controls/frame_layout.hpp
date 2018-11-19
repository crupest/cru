#pragma once

#include <initializer_list>

#include "ui/control.hpp"

namespace cru::ui::controls
{
    class FrameLayout : public Control
    {
    public:
        static constexpr auto control_type = L"FrameLayout";

        static FrameLayout* Create(const std::initializer_list<Control*>& children = std::initializer_list<Control*>{})
        {
            const auto layout = new FrameLayout();
            for (auto child : children)
                layout->AddChild(child);
            return layout;
        }

    protected:
        FrameLayout();
    public:
        FrameLayout(const FrameLayout& other) = delete;
        FrameLayout(FrameLayout&& other) = delete;
        FrameLayout& operator=(const FrameLayout& other) = delete;
        FrameLayout& operator=(FrameLayout&& other) = delete;
        ~FrameLayout() override;

        StringView GetControlType() const override final;
    };
}