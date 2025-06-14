#include "cru/platform/gui/osx/Cursor.h"
#include "CursorPrivate.h"

#include "cru/platform/gui/osx/Resource.h"
#include "cru/platform/gui/Cursor.h"
#include "cru/platform/gui/UiApplication.h"

#include <memory>

namespace cru::platform::gui::osx {
namespace details {
OsxCursorPrivate::OsxCursorPrivate(OsxCursor* cursor, SystemCursorType cursor_type) {
  cursor_ = cursor;

  switch (cursor_type) {
    case SystemCursorType::Arrow:
      ns_cursor_ = [NSCursor arrowCursor];
      break;
    case SystemCursorType::Hand:
      ns_cursor_ = [NSCursor pointingHandCursor];
      break;
    case SystemCursorType::IBeam:
      ns_cursor_ = [NSCursor IBeamCursor];
      break;
    default:
      throw Exception(u"Unknown system cursor type.");
  }
}

OsxCursorPrivate::~OsxCursorPrivate() {}
}

OsxCursor::OsxCursor(IUiApplication* ui_application, SystemCursorType cursor_type)
    : OsxGuiResource(ui_application) {
  p_ = std::make_unique<details::OsxCursorPrivate>(this, cursor_type);
}

OsxCursor::~OsxCursor() {}

namespace details {
class OsxCursorManagerPrivate {
  friend OsxCursorManager;

 public:
  explicit OsxCursorManagerPrivate(OsxCursorManager* cursor_manager);

  CRU_DELETE_COPY(OsxCursorManagerPrivate)
  CRU_DELETE_MOVE(OsxCursorManagerPrivate)

  ~OsxCursorManagerPrivate();

 private:
  OsxCursorManager* cursor_manager_;

  std::shared_ptr<OsxCursor> arrow_cursor_;
  std::shared_ptr<OsxCursor> hand_cursor_;
  std::shared_ptr<OsxCursor> ibeam_cursor_;
};

OsxCursorManagerPrivate::OsxCursorManagerPrivate(OsxCursorManager* cursor_manager) {
  cursor_manager_ = cursor_manager;
  arrow_cursor_ =
      std::make_shared<OsxCursor>(cursor_manager->GetUiApplication(), SystemCursorType::Arrow);
  hand_cursor_ =
      std::make_shared<OsxCursor>(cursor_manager->GetUiApplication(), SystemCursorType::Hand);
  ibeam_cursor_ =
      std::make_shared<OsxCursor>(cursor_manager->GetUiApplication(), SystemCursorType::IBeam);
}

OsxCursorManagerPrivate::~OsxCursorManagerPrivate() {}
}

OsxCursorManager::OsxCursorManager(IUiApplication* ui_application)
    : OsxGuiResource(ui_application) {
  p_ = std::make_unique<details::OsxCursorManagerPrivate>(this);
}

OsxCursorManager::~OsxCursorManager() {}

std::shared_ptr<ICursor> OsxCursorManager::GetSystemCursor(SystemCursorType type) {
  switch (type) {
    case SystemCursorType::Arrow:
      return p_->arrow_cursor_;
    case SystemCursorType::Hand:
      return p_->hand_cursor_;
    case SystemCursorType::IBeam:
      return p_->ibeam_cursor_;
    default:
      throw Exception(u"Unknown system cursor type.");
  }
}
}
