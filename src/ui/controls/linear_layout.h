#pragma once

#include "ui/control.h"

namespace cru::ui::controls
{
    class LinearLayout : public Control
    {
    public:
        static constexpr auto control_type = L"LinearLayout";

        enum class Orientation
        {
            Horizontal,
            Vertical
        };

        static LinearLayout* Create(const Orientation orientation = Orientation::Vertical, const std::initializer_list<Control*>& children = std::initializer_list<Control*>())
        {
            const auto linear_layout = new LinearLayout(orientation);
                        for (const auto control : children)
                linear_layout->AddChild(control);
            return linear_layout;
        }

    protected:
        explicit LinearLayout(Orientation orientation = Orientation::Vertical);

    public:
        LinearLayout(const LinearLayout& other) = delete;
        LinearLayout(LinearLayout&& other) = delete;
        LinearLayout& operator=(const LinearLayout& other) = delete;
        LinearLayout& operator=(LinearLayout&& other) = delete;
        ~LinearLayout() override = default;

        StringView GetControlType() const override final;

    protected:
        Size OnMeasureContent(const Size& available_size) override;
        void OnLayoutContent(const Rect& rect) override;

    private:
        Orientation orientation_;
    };
}
