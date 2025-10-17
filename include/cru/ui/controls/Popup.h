#pragma once
#include "RootControl.h"

#include "cru/platform/gui/Base.h"

#include <memory>

namespace cru::ui::controls {
class CRU_UI_API Popup : public RootControl {
 public:
  static constexpr std::string_view kControlType = "Popup";

  explicit Popup(Control* attached_control = nullptr);

  CRU_DELETE_COPY(Popup)
  CRU_DELETE_MOVE(Popup)

  ~Popup() override;

  std::string GetControlType() const override { return std::string(kControlType); }
};
}  // namespace cru::ui::controls
