#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "pre.hpp"

#include "ui/control.hpp"

namespace cru::ui::controls
{
    // Min length of main side in layout params is of no meaning.
    // All children will layout from start and redundant length is blank.
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
        Size OnMeasureContent(const Size& available_size, const AdditionalMeasureInfo& additional_info) override;
        void OnLayoutContent(const Rect& rect, const AdditionalLayoutInfo& additional_info) override;

    private:
        Orientation orientation_;
    };
}
