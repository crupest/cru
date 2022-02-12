#pragma once
#include "IBorderControl.h"
#include "ICheckableControl.h"
#include "NoChildControl.h"
#include "cru/ui/render/BorderRenderObject.h"

namespace cru::ui::controls {
class CRU_UI_API CheckBox : public NoChildControl,
                            public virtual IBorderControl,
                            public virtual ICheckableControl {
 public:
  CheckBox();
  ~CheckBox() override;

  render::RenderObject* GetRenderObject() const override {
    return container_render_object_.get();
  }

  bool IsChecked() const override { return checked_; }
  void SetChecked(bool checked) override;

  IEvent<bool>* CheckedChangeEvent() override { return &checked_change_event_; }

  void ApplyBorderStyle(const style::ApplyBorderStyleInfo& style) override;

 private:
  bool checked_ = false;
  Event<bool> checked_change_event_;

  std::unique_ptr<render::BorderRenderObject> container_render_object_;
};
}  // namespace cru::ui::controls
