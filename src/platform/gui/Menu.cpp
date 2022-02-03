#include "cru/platform/gui/Menu.hpp"

namespace cru::platform::gui {
std::vector<IMenuItem*> IMenu::GetItems() {
  std::vector<IMenuItem*> items;
  for (int i = 0; i < GetItemCount(); ++i) {
    items.push_back(GetItemAt(i));
  }
  return items;
}
}  // namespace cru::platform::gui
