#pragma once
#include "../Base.h"

#include "../events/KeyEventArgs.h"
#include "cru/base/Base.h"
#include "cru/base/Event.h"
#include <cru/platform/gui/Input.h>

#include <cstddef>
#include <functional>
#include <optional>
#include <unordered_map>
#include <vector>

namespace cru::ui::helper {

class ShortcutKeyBind {
 public:
  ShortcutKeyBind(
      platform::gui::KeyCode key,
      platform::gui::KeyModifier modifier = platform::gui::KeyModifiers::None)
      : key_(key), modifier_(modifier) {}

  CRU_DEFAULT_COPY(ShortcutKeyBind)
  CRU_DEFAULT_MOVE(ShortcutKeyBind)

  ~ShortcutKeyBind() = default;

  platform::gui::KeyCode GetKey() const { return key_; }
  platform::gui::KeyModifier GetModifier() const { return modifier_; }

  ShortcutKeyBind AddModifier(platform::gui::KeyModifier modifier) const {
    return ShortcutKeyBind(key_, modifier_ | modifier);
  }

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

  std::string ToString() const {
    std::string result = "(";
    result += platform::gui::ToString(modifier_);
    result += ")";
    result += platform::gui::ToString(key_);
    return result;
  }

 private:
  platform::gui::KeyCode key_;
  platform::gui::KeyModifier modifier_;
};

inline std::string ToString(const ShortcutKeyBind& key_bind) {
  return key_bind.ToString();
}
}  // namespace cru::ui::helper

namespace std {
template <>
struct hash<cru::ui::helper::ShortcutKeyBind> {
  std::size_t operator()(const cru::ui::helper::ShortcutKeyBind& value) const {
    std::size_t result = 0;
    cru::hash_combine(result, static_cast<int>(value.GetKey()));
    cru::hash_combine(result, value.GetModifier());
    return result;
  }
};
}  // namespace std

namespace cru::ui::helper {
struct Shortcut {
  // Just for debug.
  std::string name;
  ShortcutKeyBind key_bind;
  // Return true if it consumes the shortcut. Or return false if it does not
  // handle the shortcut.
  std::function<bool()> handler;
};

struct ShortcutInfo {
  int id;
  std::string name;
  ShortcutKeyBind key_bind;
  std::function<bool()> handler;
};

class CRU_UI_API ShortcutHub : public Object {
 private:
  constexpr static auto kLogTag = "ShortcutHub";

 public:
  ShortcutHub() = default;
  ~ShortcutHub() override = default;

  int RegisterShortcut(std::string name, ShortcutKeyBind bind,
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

  IEvent<events::KeyEventArgs&>* FallbackKeyEvent() { return &fallback_event_; }

  void Install(controls::Control* control);
  void Uninstall();

 private:
  void OnKeyDown(events::KeyEventArgs& event);

 private:
  std::unordered_map<ShortcutKeyBind, std::vector<ShortcutInfo>> map_;

  std::vector<ShortcutInfo> empty_list_;

  int current_id_ = 1;

  Event<events::KeyEventArgs&> fallback_event_;

  EventHandlerRevokerListGuard event_guard_;
};
}  // namespace cru::ui::helper
