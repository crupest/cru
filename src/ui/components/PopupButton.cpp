#include "cru/ui/components/PopupButton.h"
#include "cru/ui/components/Menu.h"
#include "cru/ui/helper/ClickDetector.h"

namespace cru::ui::components {
PopupMenuTextButton::PopupMenuTextButton() : popup_menu_(&button_) {
  button_.SetChild(&button_text_);
  button_.ClickEvent()->AddHandler([this](const helper::ClickEventArgs& args) {
    popup_menu_.SetPosition(args.GetDownPointOfScreen());
    popup_menu_.Show();
  });
}

PopupMenuTextButton::~PopupMenuTextButton() {}

void PopupMenuTextButton::SetMenuItems(std::vector<std::string> items) {
  popup_menu_.GetMenu()->ClearItems();
  for (Index i = 0; i < items.size(); i++) {
    popup_menu_.GetMenu()->AddTextItem(
        std::move(items[i]), [this, i] { menu_item_selected_event_.Raise(i); });
  }
}

PopupMenuIconButton::PopupMenuIconButton() : popup_menu_(&button_) {
  button_.ClickEvent()->AddHandler([this](const helper::ClickEventArgs& args) {
    popup_menu_.SetPosition(args.GetDownPointOfScreen());
    popup_menu_.Show();
  });
}

PopupMenuIconButton::~PopupMenuIconButton() {}

void PopupMenuIconButton::SetMenuItems(std::vector<std::string> items) {
  popup_menu_.GetMenu()->ClearItems();
  for (Index i = 0; i < items.size(); i++) {
    popup_menu_.GetMenu()->AddTextItem(
        std::move(items[i]), [this, i] { menu_item_selected_event_.Raise(i); });
  }
}
}  // namespace cru::ui::components
