#pragma once
#include "ContentControl.h"

namespace cru::ui::controls {
class CRU_UI_API Container : public ContentControl {
  static constexpr StringView control_type = u"Container";

 protected:
  Container();

 public:
  CRU_DELETE_COPY(Container)
  CRU_DELETE_MOVE(Container)

  ~Container() override;

 public:
  String GetControlType() const final { return control_type.ToString(); }

  render::RenderObject* GetRenderObject() const override;

 private:
  std::unique_ptr<render::BorderRenderObject> render_object_;
};
}  // namespace cru::ui::controls
