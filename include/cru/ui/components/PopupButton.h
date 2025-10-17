#pragma once
#include "Component.h"
#include "cru/ui/Base.h"
#include "cru/ui/components/Menu.h"
#include "cru/ui/controls/Button.h"
#include "cru/ui/controls/IconButton.h"
#include "cru/ui/controls/Popup.h"
#include "cru/ui/controls/TextBlock.h"

namespace cru::ui::components {
class CRU_UI_API PopupMenuTextButton : public Component {
 public:
  PopupMenuTextButton();
  ~PopupMenuTextButton() override;

 public:
  ui::controls::Control* GetRootControl() override { return &button_; }

  ui::controls::Button* GetButton() { return &button_; }

  std::string GetButtonText() { return button_text_.GetText(); }
  void SetButtonText(std::string text) { button_text_.SetText(std::move(text)); }

  void SetButtonTextColor(const Color& color) {
    button_text_.SetTextColor(color);
  }

  void SetMenuItems(std::vector<std::string> items);

  IEvent<Index>* MenuItemSelectedEvent() { return &menu_item_selected_event_; }

 private:
  ui::controls::Button button_;
  ui::controls::TextBlock button_text_;
  PopupMenu popup_menu_;

  Event<Index> menu_item_selected_event_;
};

class CRU_UI_API PopupMenuIconButton : public Component {
 public:
  PopupMenuIconButton();
  ~PopupMenuIconButton() override;

 public:
  ui::controls::Control* GetRootControl() override { return &button_; }

  ui::controls::IconButton* GetButton() { return &button_; }

  void SetMenuItems(std::vector<std::string> items);

  IEvent<Index>* MenuItemSelectedEvent() { return &menu_item_selected_event_; }

 private:
  ui::controls::IconButton button_;
  PopupMenu popup_menu_;

  Event<Index> menu_item_selected_event_;
};
}  // namespace cru::ui::components
