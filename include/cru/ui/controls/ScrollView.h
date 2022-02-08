#pragma once
#include "ContentControl.h"
#include "cru/common/Base.h"
#include "cru/ui/render/RenderObject.h"
#include "cru/ui/render/ScrollRenderObject.h"

#include <memory>
#include <string_view>

namespace cru::ui::controls {
class CRU_UI_API ScrollView : public ContentControl {
 public:
  static ScrollView* Create() { return new ScrollView(); }

  static constexpr StringView control_type = u"ScrollView";

  ScrollView();

  CRU_DELETE_COPY(ScrollView)
  CRU_DELETE_MOVE(ScrollView)

  ~ScrollView() override = default;

 public:
  String GetControlType() const override { return control_type.ToString(); }

  render::RenderObject* GetRenderObject() const override;

 private:
  std::unique_ptr<render::ScrollRenderObject> scroll_render_object_;
};
}  // namespace cru::ui::controls