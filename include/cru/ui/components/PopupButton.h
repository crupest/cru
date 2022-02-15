#pragma once
#include "Component.h"
#include "cru/ui/components/Menu.h"
#include "cru/ui/controls/Button.h"
#include "cru/ui/controls/Popup.h"
#include "cru/ui/controls/TextBlock.h"

namespace cru::ui::components {
class PopupMenuTextButton : public Component {
 public:
  PopupMenuTextButton();
  ~PopupMenuTextButton() override;

 public:
  ui::controls::Control* GetRootControl() override { return &button_; }

  String GetButtonText() { return button_text_.GetText(); }
  void SetButtonText(String text) { button_text_.SetText(std::move(text)); }

  void SetMenuItems(std::vector<String> items);

  IEvent<Index>* MenuItemSelectedEvent() { return &menu_item_selected_event_; }

 private:
  ui::controls::Button button_;
  ui::controls::TextBlock button_text_;
  PopupMenu popup_menu_;

  Event<Index> menu_item_selected_event_;
};
}  // namespace cru::ui::components
