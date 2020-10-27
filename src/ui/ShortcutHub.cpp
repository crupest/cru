#include "cru/ui/ShortcutHub.hpp"
#include <algorithm>
#include <iterator>
#include <optional>

namespace cru::ui {
int ShortcutHub::RegisterShortcut(std::u16string name, ShortcutKeyBind bind,
                                  std::function<bool()> handler) {
  const int id = current_id_++;
  ShortcutInfo info{id, std::move(name), bind, std::move(handler)};
  map_[bind].push_back(std::move(info));
  return id;
}

void ShortcutHub::UnregisterShortcut(int id) {
  if (id <= 0) return;
  for (auto& pair : map_) {
    auto& list = pair.second;
    auto result =
        std::find_if(list.cbegin(), list.cend(),
                     [id](const ShortcutInfo& info) { return info.id == id; });
    if (result != list.cend()) {
      list.erase(result);
    }
  }
}

std::vector<ShortcutInfo> ShortcutHub::GetAllShortcuts() const {
  std::vector<ShortcutInfo> result;

  for (const auto& pair : map_) {
    std::copy(pair.second.cbegin(), pair.second.cend(),
              std::back_inserter(result));
  }

  return result;
}

std::optional<ShortcutInfo> ShortcutHub::GetShortcut(int id) const {
  for (auto& pair : map_) {
    auto& list = pair.second;
    auto result =
        std::find_if(list.cbegin(), list.cend(),
                     [id](const ShortcutInfo& info) { return info.id == id; });
    if (result != list.cend()) {
      return *result;
    }
  }
  return std::nullopt;
}

const std::vector<ShortcutInfo>& ShortcutHub::GetShortcutByKeyBind(
    const ShortcutKeyBind& key_bind) const {
  auto result = map_.find(key_bind);
  if (result != map_.cend()) return result->second;
  return empty_list_;
}
}  // namespace cru::ui
