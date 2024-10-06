#pragma once
#include "cru/platform/gui/Base.h"
#include "cru/ui/controls/RootControl.h"

#include "cru/base/Base.h"

namespace cru::ui::controls {
class CRU_UI_API Window final : public RootControl {
 public:
  static constexpr StringView control_type = u"Window";

 public:
  explicit Window(Control* attached_control = nullptr);
  CRU_DELETE_COPY(Window)
  CRU_DELETE_MOVE(Window)
  ~Window() override;

 public:
  String GetControlType() const final { return control_type.ToString(); }
};
}  // namespace cru::ui::controls
