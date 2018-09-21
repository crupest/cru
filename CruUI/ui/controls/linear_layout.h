#pragma once

#include "ui/control.h"

namespace cru::ui::controls
{
    class LinearLayout : public Control
    {
    public:
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
