#pragma once
#include "cru/ui/controls/RootControl.h"

#include "cru/base/Base.h"

namespace cru::ui::controls {
class CRU_UI_API Window final : public RootControl {
 public:
  static constexpr std::string_view control_type = "Window";

 public:
  explicit Window(Control* attached_control = nullptr);
  CRU_DELETE_COPY(Window)
  CRU_DELETE_MOVE(Window)
  ~Window() override;

 public:
  std::string GetControlType() const final { return std::string(control_type); }
};
}  // namespace cru::ui::controls
