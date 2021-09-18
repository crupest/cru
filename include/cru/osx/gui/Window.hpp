#pragma once
#include "Resource.hpp"
#include "cru/platform/gui/Base.hpp"
#include "cru/platform/gui/Window.hpp"

namespace cru::platform::gui::osx {
class OsxWindow : public OsxGuiResource, public virtual INativeWindow {
 public:
  OsxWindow();

  CRU_DELETE_COPY(OsxWindow)
  CRU_DELETE_MOVE(OsxWindow)

  ~OsxWindow() override;

 private:
};
}  // namespace cru::platform::gui::osx
