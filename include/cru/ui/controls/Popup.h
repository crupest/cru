#pragma once
#include "RootControl.h"

#include "cru/gui/Base.h"

#include <memory>

namespace cru::ui::controls {
class CRU_UI_API Popup : public RootControl {
 public:
  static constexpr StringView kControlType = u"Popup";

  explicit Popup(Control* attached_control = nullptr);


  ~Popup() override;

  String GetControlType() const override { return kControlType.ToString(); }
};
}  // namespace cru::ui::controls
