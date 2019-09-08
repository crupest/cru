#include "cru/win/native/cursor.hpp"

#include "cru/common/format.hpp"
#include "cru/common/logger.hpp"
#include "cru/win/native/exception.hpp"

#include <stdexcept>

namespace cru::platform::native::win {
WinCursor::WinCursor(HCURSOR handle, bool auto_delete) {
  handle_ = handle;
  auto_delete_ = auto_delete;
}

WinCursor::~WinCursor() {
  if (auto_delete_) {
    if (!::DestroyCursor(handle_)) {
      // This is not a fetal error but might still need notice.
      log::Warn(L"Failed to destroy a cursor. Last error code: {}",
                ::GetLastError());
    }
  }
}

namespace {
WinCursor* LoadWinCursor(const wchar_t* name) {
  const auto handle = static_cast<HCURSOR>(::LoadImageW(
      NULL, name, IMAGE_CURSOR, SM_CYCURSOR, SM_CYCURSOR, LR_SHARED));
  if (handle == NULL) {
    throw Win32Error(::GetLastError(), "Failed to get system cursor.");
  }
  return new WinCursor(handle, false);
}
}  // namespace

WinCursorManager::WinCursorManager()
    : sys_arrow_(LoadWinCursor(IDC_ARROW)),
      sys_hand_(LoadWinCursor(IDC_HAND)) {}

std::shared_ptr<WinCursor> WinCursorManager::GetSystemWinCursor(
    SystemCursor type) {
  switch (type) {
    case SystemCursor::Arrow:
      return sys_arrow_;
    case SystemCursor::Hand:
      return sys_hand_;
    default:
      throw std::runtime_error("Unknown system cursor value.");
  }
}
}  // namespace cru::platform::native::win
