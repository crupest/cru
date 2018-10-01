#include "border.h"

#include "graph/graph.h"

namespace cru::ui::controls
{
    using graph::CreateSolidBrush;

    Border::Border() : Control(true), border_delegate_(this)
    {

    }

    void Border::SetDrawBorder(const bool draw_border)
    {
        draw_border_ = draw_border;
        Repaint();
    }

    void Border::OnDraw(ID2D1DeviceContext* device_context)
    {
        if (draw_border_)
        {
            border_delegate_.Draw(device_context, GetSize());
        }
    }

    Size Border::OnMeasure(const Size& available_size)
    {
        return Control::DefaultMeasureWithPadding(available_size, border_delegate_.GetBorderThickness());
    }

    void Border::OnLayout(const Rect& rect)
    {
        Control::DefaultLayoutWithPadding(rect, border_delegate_.GetBorderThickness());
    }
}
