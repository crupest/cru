#pragma once

#include "../native_resource.hpp"

#include <memory>

namespace cru::platform::native {
struct ICursor : public virtual INativeResource {};

enum class SystemCursorType {
  Arrow,
  Hand,
};

struct ICursorManager : public virtual INativeResource {
  virtual std::shared_ptr<ICursor> GetSystemCursor(SystemCursorType type) = 0;

  // TODO: Add method to create cursor.
};
}  // namespace cru::platform::native
