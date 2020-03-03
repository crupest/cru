#pragma once
#include "../no_child_control.hpp"

#include <memory>

namespace cru::ui::render {
class BorderRenderObject;
class StackLayoutRenderObject;
class TextRenderObject;
class CanvasRenderObject;
}  // namespace cru::ui::render

namespace cru::ui::controls {
class TextBox : public NoChildControl {
 public:
  static constexpr std::string_view control_type = "TextBox";

 protected:
  TextBox();

 public:
  CRU_DELETE_COPY(TextBox)
  CRU_DELETE_MOVE(TextBox)

  ~TextBox() override;

  std::string_view GetControlType() const final { return control_type; }

 private:
  std::unique_ptr<render::BorderRenderObject> border_render_object_;
  std::unique_ptr<render::StackLayoutRenderObject> stack_layout_render_object_;
  std::unique_ptr<render::TextRenderObject> text_render_object_;
  std::unique_ptr<render::CanvasRenderObject> caret_render_object_;
};
}  // namespace cru::ui::controls
