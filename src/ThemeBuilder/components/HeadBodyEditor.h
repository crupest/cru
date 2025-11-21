#pragma once
#include "Editor.h"
#include "LabeledMixin.h"
#include "cru/base/Event.h"
#include "cru/ui/controls/Container.h"
#include "cru/ui/controls/FlexLayout.h"
#include "cru/ui/controls/IconButton.h"

namespace cru::theme_builder::components {
class HeadBodyEditor : public Editor, public LabeledMixin {
 public:
  HeadBodyEditor();
  ~HeadBodyEditor() override;

 public:
  ui::controls::Control* GetRootControl() override { return &border_; }

  ui::controls::FlexLayout* GetContainer() { return &container_; }
  ui::controls::FlexLayout* GetHeadContainer() { return &head_container_; }

  IEvent<std::nullptr_t>* RemoveEvent() { return &remove_event_; }

 private:
  ui::controls::Container border_;
  ui::controls::FlexLayout container_;
  ui::controls::FlexLayout head_container_;
  ui::controls::IconButton remove_button_;

  Event<std::nullptr_t> remove_event_;
};
}  // namespace cru::theme_builder::components
