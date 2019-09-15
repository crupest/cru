#pragma once
#include "../content_control.hpp"

namespace cru::ui::render {
class BorderRenderObject;
}

namespace cru::ui::controls {
class Container : public ContentControl {
  static constexpr auto control_type = L"Container";

 protected:
  Container();

 public:
  CRU_DELETE_COPY(Container)
  CRU_DELETE_MOVE(Container)

  ~Container() override;

 public:
  std::wstring_view GetControlType() const override final {
    return control_type;
  }

  render::RenderObject* GetRenderObject() const override;

 private:
  std::unique_ptr<render::BorderRenderObject> render_object_;
};
}  // namespace cru::ui::controls
