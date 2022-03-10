#pragma once
#include "conditions/ConditionEditor.h"
#include "cru/ui/components/Component.h"
#include "cru/ui/controls/Button.h"
#include "cru/ui/controls/Control.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/TextBlock.h"
#include "cru/ui/style/StyleRule.h"
#include "stylers/StylerEditor.h"

namespace cru::theme_builder::components {
class StyleRuleEditor : public ui::components::Component {
 public:
  StyleRuleEditor();

  CRU_DELETE_COPY(StyleRuleEditor)
  CRU_DELETE_MOVE(StyleRuleEditor)

  ~StyleRuleEditor() override;

 public:
  ui::controls::Control* GetRootControl() override { return &container_; }

  ui::style::StyleRule GetValue() const;
  void SetValue(const ui::style::StyleRule& style_rule,
                bool trigger_change = true);

  IEvent<std::nullptr_t>* ChangeEvent() { return &change_event_; }
  IEvent<std::nullptr_t>* RemoveEvent() { return &remove_event_; }

 private:
  ui::controls::Container container_;
  ui::controls::FlexLayout main_layout_;
  ui::controls::TextBlock label_;
  ui::controls::FlexLayout head_layout_;
  ui::controls::Button remove_button_;
  ui::controls::TextBlock remove_button_text_;
  ui::controls::FlexLayout body_layout_;
  std::unique_ptr<components::conditions::ConditionEditor> condition_editor_;
  std::unique_ptr<components::stylers::StylerEditor> styler_editor_;

  Event<std::nullptr_t> change_event_;
  Event<std::nullptr_t> remove_event_;
};
}  // namespace cru::theme_builder::components
