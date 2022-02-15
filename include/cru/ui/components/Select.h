#pragma once
#include "Component.h"
#include "Menu.h"
#include "cru/ui/controls/Button.h"
#include "cru/ui/controls/TextBlock.h"

namespace cru::ui::components {
class CRU_UI_API Select : public Component {
 public:
  Select();
  ~Select() override;

 public:
  ui::controls::Control* GetRootControl() override { return &button_; }

  std::vector<String> GetItems() { return items_; }
  void SetItems(std::vector<String> items);

  Index GetSelectedIndex() { return selected_index_; }
  void SetSelectedIndex(Index index);

  IEvent<Index>* ItemSelectedEvent() { return &item_selected_event_; }

 private:
  Index selected_index_;
  std::vector<String> items_;

  ui::controls::Button button_;
  ui::controls::TextBlock button_text_;
  PopupMenu popup_menu_;

  Event<Index> item_selected_event_;
};
}  // namespace cru::ui::components
