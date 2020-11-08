#pragma once
#include "ContentControl.hpp"

namespace cru::ui::controls {
class Container : public ContentControl {
  static constexpr std::u16string_view control_type = u"Container";

 protected:
  Container();

 public:
  CRU_DELETE_COPY(Container)
  CRU_DELETE_MOVE(Container)

  ~Container() override;

 public:
  std::u16string_view GetControlType() const final { return control_type; }

  render::RenderObject* GetRenderObject() const override;

 protected:
  void OnChildChanged(Control* old_child, Control* new_child) override;

 private:
  std::unique_ptr<render::BorderRenderObject> render_object_;
};
}  // namespace cru::ui::controls
