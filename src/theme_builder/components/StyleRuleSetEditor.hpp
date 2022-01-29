#pragma once
#include "cru/ui/components/Component.hpp"
#include "cru/ui/controls/Control.hpp"
#include "cru/ui/controls/FlexLayout.hpp"

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
