#pragma once

#include "ui/control.h"

namespace cru::ui::controls
{
    class LinearLayout : public Control
    {
    private:
        constexpr static auto alignment_key = L"linear_layout_alignment";

    public:
        static Alignment GetAlignment(Control* control)
        {
            return control->GetAdditionalProperty<Alignment>(alignment_key).value_or(Alignment::Center);
        }

        static void SetAlignment(Control* control, Alignment alignment)
        {
            control->SetAdditionalProperty(alignment_key, alignment);
        }

        enum class Orientation
        {
            Horizontal,
            Vertical
        };

        static LinearLayout* Create(const Orientation orientation = Orientation::Vertical)
        {
            return new LinearLayout(orientation);
        }

    protected:
        explicit LinearLayout(Orientation orientation = Orientation::Vertical);

    public:
        LinearLayout(const LinearLayout& other) = delete;
        LinearLayout(LinearLayout&& other) = delete;
        LinearLayout& operator=(const LinearLayout& other) = delete;
        LinearLayout& operator=(LinearLayout&& other) = delete;
        ~LinearLayout() override = default;

    protected:
        Size OnMeasure(const Size& available_size) override;
        void OnLayout(const Rect& rect) override;

    private:
        Orientation orientation_;
    };
}
