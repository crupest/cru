#include "cru/platform/gui/win/Cursor.h"

#include "cru/base/log/Logger.h"
#include "cru/platform/gui/win/Exception.h"

#include <stdexcept>

namespace cru::platform::gui::win {
WinCursor::WinCursor(HCURSOR handle, bool auto_destroy) {
  handle_ = handle;
  auto_destroy_ = auto_destroy;
}

WinCursor::~WinCursor() {
  if (auto_destroy_) {
    if (!::DestroyCursor(handle_)) {
      // This is not a fetal error but might still need notice because it may
      // cause leak.
      CRU_LOG_TAG_WARN(u"Failed to destroy a cursor. Last error code: {}",
                   ::GetLastError());
    }
  }
}

namespace {
WinCursor* LoadWinCursor(const wchar_t* name) {
  const auto handle = static_cast<HCURSOR>(::LoadImageW(
      NULL, name, IMAGE_CURSOR, SM_CYCURSOR, SM_CYCURSOR, LR_SHARED));
  if (handle == NULL) {
    throw Win32Error(::GetLastError(), u"Failed to load system cursor.");
  }
  return new WinCursor(handle, false);
}
}  // namespace

WinCursorManager::WinCursorManager()
    : sys_arrow_(LoadWinCursor(IDC_ARROW)),
      sys_hand_(LoadWinCursor(IDC_HAND)),
      sys_ibeam_(LoadWinCursor(IDC_IBEAM)) {}

std::shared_ptr<WinCursor> WinCursorManager::GetSystemWinCursor(
    SystemCursorType type) {
  switch (type) {
    case SystemCursorType::Arrow:
      return sys_arrow_;
    case SystemCursorType::Hand:
      return sys_hand_;
    case SystemCursorType::IBeam:
      return sys_ibeam_;
    default:
      throw std::runtime_error("Unknown system cursor value.");
  }
}
}  // namespace cru::platform::gui::win
