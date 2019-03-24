#pragma once
#include "pre.hpp"

#include <memory>

#include "ui/layout_control.hpp"

namespace cru::ui::render {
class FlexLayoutRenderObject;
}

namespace cru::ui::controls {

class FlexLayout : public LayoutControl {
 public:
  static constexpr auto control_type = L"FlexLayout";

  static FlexLayout* Create() { return new FlexLayout(); }

 protected:
  FlexLayout();

 public:
  FlexLayout(const FlexLayout& other) = delete;
  FlexLayout(FlexLayout&& other) = delete;
  FlexLayout& operator=(const FlexLayout& other) = delete;
  FlexLayout& operator=(FlexLayout&& other) = delete;
  ~FlexLayout() override = default;

  StringView GetControlType() const override final { return control_type; }

  render::RenderObject* GetRenderObject() const override;

 protected:
  void OnAddChild(Control* child, int position) override;
  void OnRemoveChild(Control* child, int position) override;

 private:
  std::shared_ptr<render::FlexLayoutRenderObject> render_object_;
};
}  // namespace cru::ui::controls
