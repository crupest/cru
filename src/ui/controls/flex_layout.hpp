#pragma once
#include "pre.hpp"

#include "ui/control.hpp"

namespace cru::ui::render {
class FlexLayoutRenderObject;
}

namespace cru::ui::controls {

class FlexLayout : public Layout {
 public:
  static constexpr auto control_type = L"FlexLayout";

 public:
  FlexLayout();
  FlexLayout(const FlexLayout& other) = delete;
  FlexLayout(FlexLayout&& other) = delete;
  FlexLayout& operator=(const FlexLayout& other) = delete;
  FlexLayout& operator=(FlexLayout&& other) = delete;
  ~FlexLayout() override;

  StringView GetControlType() const override final { return control_type; }

  render::RenderObject* GetRenderObject() const override {
    return render_object_;
  }

 protected:
  void OnAddChild(Control* child, int position) override;
  void OnRemoveChild(Control* child, int position) override;

 private:
  render::FlexLayoutRenderObject* render_object_;
};
}  // namespace cru::ui::controls
