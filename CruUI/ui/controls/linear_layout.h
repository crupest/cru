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

        explicit LinearLayout(Orientation orientation = Orientation::Vertical);

    protected:
        Size OnMeasure(const Size& available_size) override;
        void OnLayout(const Rect& rect) override;

    private:
        Orientation orientation_;
    };
}
