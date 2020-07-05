#pragma once
#include "../NoChildControl.hpp"
#include "Base.hpp"

#include <memory>

namespace cru::ui::controls {
template <typename TControl>
class TextControlService;

class TextBox : public NoChildControl {
 public:
  static constexpr std::string_view control_type = "TextBox";

  static TextBox* Create() { return new TextBox(); }

 protected:
  TextBox();

 public:
  CRU_DELETE_COPY(TextBox)
  CRU_DELETE_MOVE(TextBox)

  ~TextBox() override;

  std::string_view GetControlType() const final { return control_type; }

  render::RenderObject* GetRenderObject() const override;

  gsl::not_null<render::TextRenderObject*> GetTextRenderObject();
  render::ScrollRenderObject* GetScrollRenderObject();

  const TextBoxBorderStyle& GetBorderStyle();
  void SetBorderStyle(TextBoxBorderStyle border_style);

 protected:
  void OnMouseHoverChange(bool newHover) override;

 private:
  void UpdateBorderStyle();

 private:
  std::unique_ptr<render::BorderRenderObject> border_render_object_;
  std::unique_ptr<render::ScrollRenderObject> scroll_render_object_;
  std::unique_ptr<render::TextRenderObject> text_render_object_;

  TextBoxBorderStyle border_style_;

  std::unique_ptr<TextControlService<TextBox>> service_;
};
}  // namespace cru::ui::controls
