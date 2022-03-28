#include "cru/ui/DeleteLater.h"

#include "Helper.h"
#include "cru/platform/gui/UiApplication.h"

namespace cru::ui {
DeleteLaterImpl::~DeleteLaterImpl() {}

void DeleteLaterImpl::DeleteLater() {
  GetUiApplication()->SetImmediate([this] { delete this; });
}
}  // namespace cru::ui
