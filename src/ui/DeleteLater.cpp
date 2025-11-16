#include "cru/ui/DeleteLater.h"

#include "Helper.h"
#include "cru/base/log/Logger.h"
#include "cru/platform/gui/UiApplication.h"

namespace cru::ui {

DeleteLaterImpl::DeleteLaterImpl() : delete_later_scheduled_(false) {}

DeleteLaterImpl::~DeleteLaterImpl() {
  CRU_LOG_TAG_DEBUG("Delete later object being deleted {}.",
                    static_cast<void*>(this));
}

void DeleteLaterImpl::DeleteLater() {
  if (!delete_later_scheduled_) {
    CRU_LOG_TAG_DEBUG("Schedule delete later {}.", static_cast<void*>(this));
    GetUiApplication()->SetImmediate([this] { delete this; });
    delete_later_scheduled_ = true;
  }
}

void DeleteLaterImpl::OnPrepareDelete() {}
}  // namespace cru::ui
