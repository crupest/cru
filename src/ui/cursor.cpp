#include "cursor.hpp"

#include "exception.hpp"

namespace cru::ui {
Cursor::Cursor(HCURSOR handle, const bool auto_release)
    : handle_(handle), auto_release_(auto_release) {}

Cursor::~Cursor() {
  if (auto_release_) ::DestroyCursor(handle_);
}

namespace cursors {
Cursor::Ptr arrow{};
Cursor::Ptr hand{};
Cursor::Ptr i_beam{};

void LoadSystemCursors() {
  arrow = std::make_shared<Cursor>(::LoadCursorW(nullptr, IDC_ARROW), false);
  hand = std::make_shared<Cursor>(::LoadCursorW(nullptr, IDC_HAND), false);
  i_beam = std::make_shared<Cursor>(::LoadCursorW(nullptr, IDC_IBEAM), false);
}
}  // namespace cursors
}  // namespace cru::ui
