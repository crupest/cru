#pragma once
#include "Control.hpp"

namespace cru::ui {
class LayoutControl : public Control {
 protected:
  LayoutControl() = default;

 public:
  LayoutControl(const LayoutControl& other) = delete;
  LayoutControl(LayoutControl&& other) = delete;
  LayoutControl& operator=(const LayoutControl& other) = delete;
  LayoutControl& operator=(LayoutControl&& other) = delete;
  ~LayoutControl() override = default;

  using Control::AddChild;
  using Control::RemoveChild;
};
}  // namespace cru::ui
