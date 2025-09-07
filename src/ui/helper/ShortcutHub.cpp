#include "cru/ui/helper/ShortcutHub.h"

#include "cru/base/log/Logger.h"
#include "cru/ui/DebugFlags.h"
#include "cru/ui/controls/Control.h"

#include <algorithm>
#include <functional>
#include <iterator>
#include <optional>

namespace cru::ui::helper {
int ShortcutHub::RegisterShortcut(Shortcut shortcut) {
  const int id = current_id_++;
  map_[shortcut.key_bind].push_back({id, std::move(shortcut.name),
                                     shortcut.key_bind,
                                     std::move(shortcut.handler)});
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

void ShortcutHub::Install(controls::Control* control) {
  if (!event_guard_.IsEmpty()) {
    CRU_LOG_TAG_ERROR("Shortcut hub is already installed. Failed to install.");
    return;
  }

  event_guard_ += control->KeyDownEvent()->Bubble()->AddHandler(
      std::bind(&ShortcutHub::OnKeyDown, this, std::placeholders::_1));
}

void ShortcutHub::Uninstall() {
  if (event_guard_.IsEmpty()) {
    CRU_LOG_TAG_WARN("Shortcut hub is not installed. Failed to uninstall.");
    return;
  }

  event_guard_.Clear();
}

void ShortcutHub::OnKeyDown(events::KeyEventArgs& event) {
  ShortcutKeyBind key_bind(event.GetKeyCode(), event.GetKeyModifier());
  const auto& shortcut_list = this->GetShortcutByKeyBind(key_bind);

  bool handled = false;

  if constexpr (debug_flags::shortcut) {
    if (shortcut_list.empty()) {
      CRU_LOG_TAG_DEBUG("No shortcut for key bind {}.",
                        key_bind.ToString().ToUtf8());
    }
    CRU_LOG_TAG_DEBUG("Begin to handle shortcut for key bind {}.",
                      key_bind.ToString().ToUtf8());
  }

  for (const auto& shortcut : shortcut_list) {
    auto is_handled = shortcut.handler();
    if (is_handled) {
      if constexpr (debug_flags::shortcut) {
        CRU_LOG_TAG_DEBUG("Handle {} handled it.", shortcut.name.ToUtf8());
      }

      handled = true;
      event.SetHandled();

      break;
    } else {
      if constexpr (debug_flags::shortcut) {
        CRU_LOG_TAG_DEBUG("Handle {} didn't handle it.",
                          shortcut.name.ToUtf8());
      }
    }
  }

  if constexpr (debug_flags::shortcut) {
    if (!shortcut_list.empty()) {
      CRU_LOG_TAG_DEBUG("End handling shortcut for key bind {}.",
                        key_bind.ToString().ToUtf8());
    }
  }

  if (!handled) {
    if constexpr (debug_flags::shortcut) {
      CRU_LOG_TAG_DEBUG(
          "Raise fallback event for unhandled shortcut of key bind {}.",
          key_bind.ToString().ToUtf8());
    }
    fallback_event_.Raise(event);
  }
}
}  // namespace cru::ui::helper
