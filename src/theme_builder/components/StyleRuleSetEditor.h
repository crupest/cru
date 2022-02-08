#pragma once
#include "cru/ui/components/Component.h"
#include "cru/ui/controls/Control.h"
#include "cru/ui/controls/FlexLayout.h"

namespace cru::theme_builder {
class StyleRuleSetEditor : public ui::components::Component {
 public:
  StyleRuleSetEditor();

  CRU_DELETE_COPY(StyleRuleSetEditor)
  CRU_DELETE_MOVE(StyleRuleSetEditor)

  ~StyleRuleSetEditor() override;

 public:
  ui::controls::Control* GetRootControl() override { return main_layout_; }

 private:
  ui::controls::FlexLayout* main_layout_;
};
}  // namespace cru::theme_builder
