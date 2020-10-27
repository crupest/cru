#pragma once
#include "Base.hpp"

#include "cru/platform/native/Keyboard.hpp"

#include <functional>
#include <optional>
#include <string>
#include <string_view>
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

 private:
  platform::native::KeyCode key_;
  platform::native::KeyModifier modifier_;
};

struct ShortcutInfo {
  std::u16string name;
  ShortcutKeyBind key_bind;
  std::function<bool()> handler;
};

class ShortcutHub : public Object {
 public:
  ShortcutHub();

  CRU_DELETE_COPY(ShortcutHub)
  CRU_DELETE_MOVE(ShortcutHub)

  ~ShortcutHub() override;

  // Handler return true if it consumes the shortcut. Or return false if it does
  // not handle the shortcut. Name is just for debug.
  int RegisterShortcut(std::u16string name, ShortcutKeyBind bind,
                       std::function<bool()> handler);

  void UnregisterShortcut(int id);

  std::vector<ShortcutInfo> GetAllShortcuts() const;
  std::optional<ShortcutInfo> GetShortcut(int id) const;
  std::vector<ShortcutInfo> GetShortcutByKeyBind(
      const ShortcutKeyBind& key_bind) const;

  void Install(Control* control);
  void Uninstall();

 private:
};
}  // namespace cru::ui
