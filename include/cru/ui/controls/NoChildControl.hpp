#pragma once
#include "Control.hpp"

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

 private:
  using Control::AddChild;
  using Control::RemoveChild;
};
}  // namespace cru::ui
