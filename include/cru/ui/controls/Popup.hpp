#pragma once
#include "RootControl.hpp"

#include "cru/platform/gui/Base.hpp"

#include <memory>

namespace cru::ui::controls {
class Popup : public RootControl {
 public:
  explicit Popup(Control* attached_control = nullptr);

  CRU_DELETE_COPY(Popup)
  CRU_DELETE_MOVE(Popup)

  ~Popup() override;

 protected:
  gsl::not_null<platform::gui::INativeWindow*> CreateNativeWindow(
      gsl::not_null<host::WindowHost*> host,
      platform::gui::INativeWindow* parent) override;
};
}  // namespace cru::ui::controls
