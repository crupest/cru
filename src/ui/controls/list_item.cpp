#include "list_item.hpp"

#include "ui/ui_manager.hpp"
#include "ui/convert_util.hpp"

namespace cru::ui::controls
{
    ListItem::ListItem() : Control(true)
    {
        const auto predefine_resources = UiManager::GetInstance()->GetPredefineResources();

        brushes_[State::Normal].border_brush = predefine_resources->list_item_normal_border_brush;
        brushes_[State::Normal].fill_brush   = predefine_resources->list_item_normal_fill_brush;
        brushes_[State::Hover] .border_brush = predefine_resources->list_item_hover_border_brush;
        brushes_[State::Hover] .fill_brush   = predefine_resources->list_item_hover_fill_brush;
        brushes_[State::Select].border_brush = predefine_resources->list_item_select_border_brush;
        brushes_[State::Select].fill_brush   = predefine_resources->list_item_select_fill_brush;
    }

    StringView ListItem::GetControlType() const
    {
        return control_type;
    }

    void ListItem::SetState(const State state)
    {
        state_ = state;
        Repaint();
    }

    void ListItem::OnDrawForeground(ID2D1DeviceContext* device_context)
    {
        const auto rect = Rect(Point::Zero(), GetRect(RectRange::Padding).GetSize());
        device_context->FillRectangle(Convert(rect), brushes_[state_].fill_brush.Get());
        device_context->DrawRectangle(Convert(rect.Shrink(Thickness(0.5))), brushes_[state_].border_brush.Get(), 1);
    }

    void ListItem::OnMouseEnterCore(events::MouseEventArgs& args)
    {
        if (GetState() == State::Select)
            return;

        if (IsAnyMouseButtonDown())
            return;

        SetState(State::Hover);
    }

    void ListItem::OnMouseLeaveCore(events::MouseEventArgs& args)
    {
        if (GetState() == State::Select)
            return;

        SetState(State::Normal);
    }

    void ListItem::OnMouseClickCore(events::MouseButtonEventArgs& args)
    {
        if (args.GetMouseButton() == MouseButton::Left)
            SetState(State::Select);
    }
}
