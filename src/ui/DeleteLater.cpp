#include "cru/ui/DeleteLater.h"

#include "Helper.h"
#include "cru/platform/gui/UiApplication.h"

namespace cru::ui {

DeleteLaterImpl::DeleteLaterImpl() : delete_later_scheduled_(false) {}

DeleteLaterImpl::~DeleteLaterImpl() {}

void DeleteLaterImpl::DeleteLater() {
  if (!delete_later_scheduled_) {
    GetUiApplication()->SetImmediate([this] { delete this; });
    delete_later_scheduled_ = true;
  }
}

void DeleteLaterImpl::OnPrepareDelete() {}
}  // namespace cru::ui
