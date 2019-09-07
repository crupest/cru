#pragma once

#include "../native_resource.hpp"

#include <memory>

namespace cru::platform::native {
class Cursor : public NativeResource {
 public:
  Cursor() = default;

  CRU_DELETE_COPY(Cursor)
  CRU_DELETE_MOVE(Cursor)

  ~Cursor() override = default;
};

enum class SystemCursor {
  Arrow,
  Hand,
};

class CursorManager : public NativeResource {
 public:
  CursorManager() = default;

  CRU_DELETE_COPY(CursorManager)
  CRU_DELETE_MOVE(CursorManager)

  ~CursorManager() override = default;

 public:
  virtual std::shared_ptr<Cursor> GetSystemCursor(SystemCursor type) = 0;

  //TODO: Add method to create cursor.
};
}  // namespace cru::platform::native
