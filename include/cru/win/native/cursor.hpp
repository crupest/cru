#pragma once
#include <memory>
#include "../win_pre_config.hpp"

#include "cru/common/base.hpp"
#include "cru/platform/native/cursor.hpp"
#include "cru/win/native/platform_id.hpp"

namespace cru::platform::native::win {
class WinCursor : public Cursor {
 public:
  WinCursor(HCURSOR handle, bool auto_delete);

  CRU_DELETE_COPY(WinCursor)
  CRU_DELETE_MOVE(WinCursor)

  ~WinCursor() override;

  CRU_PLATFORMID_IMPLEMENT_WIN

 public:
  HCURSOR GetHandle() const { return handle_; }

 private:
  HCURSOR handle_;
  bool auto_delete_;
};

class WinCursorManager : public CursorManager {
 public:
  WinCursorManager();

  CRU_DELETE_COPY(WinCursorManager)
  CRU_DELETE_MOVE(WinCursorManager)

  ~WinCursorManager() override = default;

  CRU_PLATFORMID_IMPLEMENT_WIN

 public:
  std::shared_ptr<WinCursor> GetSystemWinCursor(SystemCursor type);

  std::shared_ptr<Cursor> GetSystemCursor(SystemCursor type) override {
    return std::static_pointer_cast<Cursor>(GetSystemWinCursor(type));
  }

 private:
  std::shared_ptr<WinCursor> sys_arrow_;
  std::shared_ptr<WinCursor> sys_hand_;
};
}  // namespace cru::platform::native::win
