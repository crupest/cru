#include "cru/ui/components/Select.h"

namespace cru::ui::components {
Select::Select() {
  button_.SetChild(&button_text_);
  button_.ClickEvent()->AddHandler([this](const helper::ClickEventArgs& args) {
    popup_menu_.SetPosition(args.GetDownPoint());
    popup_menu_.Show();
  });
}

Select::~Select() { button_.RemoveFromParent(); }

void Select::SetItems(std::vector<String> items) {
  popup_menu_.GetMenu()->ClearItems();
  for (Index i = 0; i < items.size(); i++) {
    popup_menu_.GetMenu()->AddTextItem(std::move(items[i]),
                                       [this, i] { SetSelectedIndex(i); });
  }
}

void Select::SetSelectedIndex(Index index) {
  selected_index_ = index;
  if (index >= 0 && index < items_.size()) {
    button_text_.SetText(items_[index]);
  } else {
    button_text_.SetText({});
  }
  item_selected_event_.Raise(index);
}
}  // namespace cru::ui::components
