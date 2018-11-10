#include "button.hpp"

#include "graph/graph.hpp"
#include "ui/ui_manager.hpp"

namespace cru::ui::controls
{
    Button::Button() : Control(true),
        normal_border_{UiManager::GetInstance()->GetPredefineResources()->button_normal_border},
        pressed_border_{UiManager::GetInstance()->GetPredefineResources()->button_press_border}
    {
        SetBordered(true);
        GetBorderProperty() = normal_border_;

        SetCursor(cursors::hand);
    }

    StringView Button::GetControlType() const
    {
        return control_type;
    }

    void Button::OnMouseClickBegin(MouseButton button)
    {
        GetBorderProperty() = pressed_border_;
        InvalidateBorder();
    }

    void Button::OnMouseClickEnd(MouseButton button)
    {
        GetBorderProperty() = normal_border_;
        InvalidateBorder();
    }
}
