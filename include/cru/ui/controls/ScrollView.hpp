#pragma once
#include "ContentControl.hpp"
#include "cru/common/Base.hpp"
#include "cru/ui/render/RenderObject.hpp"
#include "cru/ui/render/ScrollRenderObject.hpp"

#include <memory>
#include <string_view>

namespace cru::ui::controls {
class ScrollView : public ContentControl {
 public:
  static ScrollView* Create() { return new ScrollView(); }

  static constexpr std::u16string_view control_type = u"ScrollView";

  ScrollView();

  CRU_DELETE_COPY(ScrollView)
  CRU_DELETE_MOVE(ScrollView)

  ~ScrollView() override = default;

 public:
  std::u16string_view GetControlType() const override { return control_type; }

  render::RenderObject* GetRenderObject() const override;

 private:
  std::unique_ptr<render::ScrollRenderObject> scroll_render_object_;
};
}  // namespace cru::ui::controls
