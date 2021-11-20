#pragma once
#include "RootControl.hpp"

#include "cru/platform/gui/Base.hpp"

#include <memory>

namespace cru::ui::controls {
class Popup : public RootControl {
 public:
  static constexpr StringView kControlType = u"Popup";

  static Popup* Create(Control* attached_control = nullptr) {
    return new Popup(attached_control);
  }

  explicit Popup(Control* attached_control = nullptr);

  CRU_DELETE_COPY(Popup)
  CRU_DELETE_MOVE(Popup)

  ~Popup() override;

  String GetControlType() const override { return kControlType.ToString(); }

 protected:
  gsl::not_null<platform::gui::INativeWindow*> CreateNativeWindow(
      gsl::not_null<host::WindowHost*> host,
      platform::gui::INativeWindow* parent) override;
};
}  // namespace cru::ui::controls
