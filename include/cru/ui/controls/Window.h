#pragma once
#include "cru/platform/gui/Base.h"
#include "cru/ui/controls/RootControl.h"

#include "cru/common/Base.h"

namespace cru::ui::controls {
class CRU_UI_API Window final : public RootControl {
 public:
  static constexpr StringView control_type = u"Window";

 public:
  static Window* Create(Control* attached_control = nullptr);

 private:
  explicit Window(Control* attached_control);

 public:
  CRU_DELETE_COPY(Window)
  CRU_DELETE_MOVE(Window)

  ~Window() override;

 public:
  String GetControlType() const final { return control_type.ToString(); }
};
}  // namespace cru::ui::controls
