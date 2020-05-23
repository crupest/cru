#include "cru/win/native/Cursor.hpp"

#include "cru/common/Logger.hpp"
#include "cru/win/native/Exception.hpp"

#include <stdexcept>

namespace cru::platform::native::win {
WinCursor::WinCursor(HCURSOR handle, bool auto_destroy) {
  handle_ = handle;
  auto_destroy_ = auto_destroy;
}

WinCursor::~WinCursor() {
  if (auto_destroy_) {
    if (!::DestroyCursor(handle_)) {
      // This is not a fetal error but might still need notice because it may
      // cause leak.
      log::Warn("Failed to destroy a cursor. Last error code: {}",
                ::GetLastError());
    }
  }
}

namespace {
WinCursor* LoadWinCursor(const wchar_t* name) {
  const auto handle = static_cast<HCURSOR>(::LoadImageW(
      NULL, name, IMAGE_CURSOR, SM_CYCURSOR, SM_CYCURSOR, LR_SHARED));
  if (handle == NULL) {
    throw Win32Error(::GetLastError(), "Failed to load system cursor.");
  }
  return new WinCursor(handle, false);
}
}  // namespace

WinCursorManager::WinCursorManager()
    : sys_arrow_(LoadWinCursor(IDC_ARROW)),
      sys_hand_(LoadWinCursor(IDC_HAND)) {}

std::shared_ptr<WinCursor> WinCursorManager::GetSystemWinCursor(
    SystemCursorType type) {
  switch (type) {
    case SystemCursorType::Arrow:
      return sys_arrow_;
    case SystemCursorType::Hand:
      return sys_hand_;
    default:
      throw std::runtime_error("Unknown system cursor value.");
  }
}
}  // namespace cru::platform::native::win
