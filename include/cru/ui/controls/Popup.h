#pragma once
#include "RootControl.h"

#include "cru/platform/gui/Base.h"

#include <memory>

namespace cru::ui::controls {
class CRU_UI_API Popup : public RootControl {
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
};
}  // namespace cru::ui::controls
