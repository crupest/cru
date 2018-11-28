#include "popup_menu.hpp"

#include "ui/window.hpp"
#include "text_block.hpp"
#include "list_item.hpp"
#include "linear_layout.hpp"
#include "ui/events/ui_event.hpp"

namespace cru::ui::controls
{
    Window* CreatePopupMenu(const Point& anchor, const std::vector<MenuItemInfo>& items, Window* parent)
    {
        const auto popup = Window::CreatePopup(parent);

        popup->lose_focus_event.bubble.AddHandler([popup](events::FocusChangeEventArgs& args)
        {
            if (args.IsWindow())
                popup->Close();
        });

        const auto create_menu_item = [popup](const String& text, const std::function<void()>& action) -> ListItem*
        {
            auto text_block = TextBlock::Create(text);
            text_block->GetLayoutParams()->width.alignment = Alignment::Start;

            auto list_item = CreateWithLayout<ListItem>(
                LayoutSideParams::Stretch(Alignment::Center),
                LayoutSideParams::Content(Alignment::Start),
                ControlList{ text_block }
            );

            list_item->mouse_click_event.bubble.AddHandler([popup, action](events::MouseButtonEventArgs& args)
            {
                if (args.GetMouseButton() == MouseButton::Left)
                {
                    action();
                    popup->Close();
                }
            });

            return list_item;
        };

        const auto menu = LinearLayout::Create(LinearLayout::Orientation::Vertical);

        menu->SetBordered(true);

        for (const auto& item : items)
            menu->AddChild(create_menu_item(item.first, item.second));

        popup->AddChild(menu);

        popup->SetSizeFitContent();
        popup->SetWindowPosition(anchor);

        return popup;
    }
}
