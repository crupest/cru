#pragma once
#include "../Base.hpp"

#include "../events/UiEvent.hpp"
#include "cru/common/Event.hpp"
#include "cru/platform/gui/Keyboard.hpp"

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace cru::ui::helper {

class ShortcutKeyBind {
 public:
  ShortcutKeyBind(
      platform::gui::KeyCode key,
      platform::gui::KeyModifier modifier = platform::gui::KeyModifiers::none)
      : key_(key), modifier_(modifier) {}

  CRU_DEFAULT_COPY(ShortcutKeyBind)
  CRU_DEFAULT_MOVE(ShortcutKeyBind)

  ~ShortcutKeyBind() = default;

  platform::gui::KeyCode GetKey() const { return key_; }
  platform::gui::KeyModifier GetModifier() const { return modifier_; }

  bool Is(platform::gui::KeyCode key,
          platform::gui::KeyModifier modifier) const {
    return key == key_ && modifier == modifier_;
  }

  bool operator==(const ShortcutKeyBind& other) const {
    return this->key_ == other.key_ && this->modifier_ == other.modifier_;
  }

  bool operator!=(const ShortcutKeyBind& other) const {
    return !this->operator==(other);
  }

  String ToString() {
    String result = u"(";
    result += platform::gui::ToString(modifier_);
    result += u")";
    result += platform::gui::ToString(key_);
    return result;
  }

 private:
  platform::gui::KeyCode key_;
  platform::gui::KeyModifier modifier_;
};
}  // namespace cru::ui::helper

namespace std {
template <>
struct hash<cru::ui::helper::ShortcutKeyBind> {
  std::size_t operator()(const cru::ui::helper::ShortcutKeyBind& value) const {
    std::size_t result = 0;
    cru::hash_combine(result, static_cast<int>(value.GetKey()));
    cru::hash_combine(result, static_cast<int>(value.GetModifier()));
    return result;
  }
};
}  // namespace std

namespace cru::ui::helper {
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

  IEvent<event::KeyEventArgs&>* FallbackKeyEvent() { return &fallback_event_; }

  void Install(controls::Control* control);
  void Uninstall();

 private:
  void OnKeyDown(event::KeyEventArgs& event);

 private:
  std::unordered_map<ShortcutKeyBind, std::vector<ShortcutInfo>> map_;

  const std::vector<ShortcutInfo> empty_list_;

  int current_id_ = 1;

  Event<event::KeyEventArgs&> fallback_event_;

  EventRevokerListGuard event_guard_;
};
}  // namespace cru::ui::helper
