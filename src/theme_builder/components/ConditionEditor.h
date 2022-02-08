#pragma once
#include "cru/ui/components/Component.h"

namespace cru::theme_builder {
class ConditionEditor : public ui::components::Component {
 public:
  ConditionEditor();

  ~ConditionEditor() override;

 public:
  ui::controls::Control* GetRootControl() override { return nullptr; }

 private:
};
}  // namespace cru::theme_builder
