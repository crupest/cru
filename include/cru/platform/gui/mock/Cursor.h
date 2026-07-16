#pragma once

#include "Base.h"

#include <cru/platform/gui/Cursor.h>

#include <map>
#include <memory>

namespace cru::platform::gui::mock {
class CRU_PLATFORM_GUI_MOCK_API MockCursor : public MockResource,
                                             public virtual ICursor {
 public:
  explicit MockCursor(SystemCursorType type);

  SystemCursorType GetType() const { return type_; }

 private:
  SystemCursorType type_;
};

class CRU_PLATFORM_GUI_MOCK_API MockCursorManager
    : public MockResource,
      public virtual ICursorManager {
 public:
  MockCursorManager();

  std::shared_ptr<ICursor> GetSystemCursor(SystemCursorType type) override;
  std::shared_ptr<MockCursor> GetMockSystemCursor(SystemCursorType type);

 private:
  std::map<SystemCursorType, std::shared_ptr<MockCursor>> system_cursors_;
};
}  // namespace cru::platform::gui::mock
