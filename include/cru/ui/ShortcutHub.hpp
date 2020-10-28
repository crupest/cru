#pragma once
#include "Base.hpp"

#include "cru/common/Base.hpp"
#include "cru/common/Event.hpp"
#include "cru/platform/native/Keyboard.hpp"
#include "cru/ui/UiEvent.hpp"

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace cru::ui {

class ShortcutKeyBind {
 public:
  ShortcutKeyBind(platform::native::KeyCode key,
                  platform::native::KeyModifier modifier)
      : key_(key), modifier_(modifier) {}

  CRU_DEFAULT_COPY(ShortcutKeyBind)
  CRU_DEFAULT_MOVE(ShortcutKeyBind)

  ~ShortcutKeyBind() = default;

  platform::native::KeyCode GetKey() const { return key_; }
  platform::native::KeyModifier GetModifier() const { return modifier_; }

  bool Is(platform::native::KeyCode key,
          platform::native::KeyModifier modifier) const {
    return key == key_ && modifier == modifier_;
  }

  bool operator==(const ShortcutKeyBind& other) const {
    return this->key_ == other.key_ && this->modifier_ == other.modifier_;
  }

  bool operator!=(const ShortcutKeyBind& other) const {
    return !this->operator==(other);
  }

  std::u16string ToString() {
    std::u16string result = u"(";
    result += platform::native::ToString(modifier_);
    result += u")";
    result += platform::native::ToString(key_);
    return result;
  }

 private:
  platform::native::KeyCode key_;
  platform::native::KeyModifier modifier_;
};
}  // namespace cru::ui

namespace std {
template <>
struct hash<cru::ui::ShortcutKeyBind> {
  std::size_t operator()(const cru::ui::ShortcutKeyBind& value) const {
    std::size_t result = 0;
    cru::hash_combine(result, static_cast<int>(value.GetKey()));
    cru::hash_combine(result, static_cast<int>(value.GetModifier()));
    return result;
  }
};
}  // namespace std

namespace cru::ui {
struct Shortcut {
  // Just for debug.
  std::u16string name;
  ShortcutKeyBind key_bind;
  // Return true if it consumes the shortcut. Or return false if it does not
  // handle the shortcut.
  std::function<bool()> handler;
};

struct ShortcutInfo {
  int id;
  std::u16string name;
  ShortcutKeyBind key_bind;
  std::function<bool()> handler;
};

class ShortcutHub : public Object {
 public:
  ShortcutHub() = default;

  CRU_DELETE_COPY(ShortcutHub)
  CRU_DELETE_MOVE(ShortcutHub)

  ~ShortcutHub() override = default;

  int RegisterShortcut(std::u16string name, ShortcutKeyBind bind,
                       std::function<bool()> handler) {
    return RegisterShortcut({std::move(name), bind, std::move(handler)});
  }

  // Return an id used for unregistering.
  int RegisterShortcut(Shortcut shortcut);

  void UnregisterShortcut(int id);

  std::vector<ShortcutInfo> GetAllShortcuts() const;
  std::optional<ShortcutInfo> GetShortcut(int id) const;
  const std::vector<ShortcutInfo>& GetShortcutByKeyBind(
      const ShortcutKeyBind& key_bind) const;

  void Install(Control* control);
  void Uninstall();

 private:
  void OnKeyDown(event::KeyEventArgs& event);

 private:
  std::unordered_map<ShortcutKeyBind, std::vector<ShortcutInfo>> map_;

  const std::vector<ShortcutInfo> empty_list_;

  int current_id_ = 1;

  EventRevokerListGuard event_guard_;
};
}  // namespace cru::ui
