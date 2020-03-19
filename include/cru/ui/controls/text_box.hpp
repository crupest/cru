#pragma once
#include "../no_child_control.hpp"
#include "base.hpp"

namespace cru::ui::controls {
template <typename TControl>
class TextControlService;

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

  render::TextRenderObject* GetTextRenderObject();
  render::CanvasRenderObject* GetCaretRenderObject();
  std::shared_ptr<platform::graph::IBrush> GetCaretBrush();

  const TextBoxBorderStyle& GetBorderStyle();
  void SetBorderStyle(TextBoxBorderStyle border_style);

 private:
  std::unique_ptr<render::BorderRenderObject> border_render_object_;
  std::unique_ptr<render::StackLayoutRenderObject> stack_layout_render_object_;
  std::unique_ptr<render::TextRenderObject> text_render_object_;
  std::unique_ptr<render::CanvasRenderObject> caret_render_object_;

  std::shared_ptr<platform::graph::IBrush> caret_brush_;
  TextBoxBorderStyle border_style_;

  std::unique_ptr<TextControlService<TextBox>> service_;
};
}  // namespace cru::ui::controls
