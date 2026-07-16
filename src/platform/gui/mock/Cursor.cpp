#include "cru/platform/gui/mock/Cursor.h"

#include <utility>

namespace cru::platform::gui::mock {
MockCursor::MockCursor(SystemCursorType type) : type_(type) {}

MockCursorManager::MockCursorManager() {
  system_cursors_.emplace(
      SystemCursorType::Arrow,
      std::make_shared<MockCursor>(SystemCursorType::Arrow));
  system_cursors_.emplace(SystemCursorType::Hand,
                          std::make_shared<MockCursor>(SystemCursorType::Hand));
  system_cursors_.emplace(
      SystemCursorType::IBeam,
      std::make_shared<MockCursor>(SystemCursorType::IBeam));
}

std::shared_ptr<ICursor> MockCursorManager::GetSystemCursor(
    SystemCursorType type) {
  return std::static_pointer_cast<ICursor>(GetMockSystemCursor(type));
}

std::shared_ptr<MockCursor> MockCursorManager::GetMockSystemCursor(
    SystemCursorType type) {
  return system_cursors_.at(type);
}
}  // namespace cru::platform::gui::mock
