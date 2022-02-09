#pragma once
#include "Control.h"

namespace cru::ui::controls {
class CRU_UI_API NoChildControl : public Control {
 protected:
  NoChildControl() = default;

 public:
  NoChildControl(const NoChildControl& other) = delete;
  NoChildControl(NoChildControl&& other) = delete;
  NoChildControl& operator=(const NoChildControl& other) = delete;
  NoChildControl& operator=(NoChildControl&& other) = delete;
  ~NoChildControl() override = default;

 public:
  void ForEachChild(const std::function<void(Control*)>& callback) override;
};
}  // namespace cru::ui::controls
