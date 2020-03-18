#pragma once
#include "../content_control.hpp"

namespace cru::ui::controls {
class Container : public ContentControl {
  static constexpr std::string_view control_type = "Container";

 protected:
  Container();

 public:
  CRU_DELETE_COPY(Container)
  CRU_DELETE_MOVE(Container)

  ~Container() override;

 public:
  std::string_view GetControlType() const final { return control_type; }

  render::RenderObject* GetRenderObject() const override;

 private:
  std::unique_ptr<render::BorderRenderObject> render_object_;
};
}  // namespace cru::ui::controls
