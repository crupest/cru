#pragma once
#include "NoChildControl.hpp"

#include "IBorderControl.hpp"
#include "TextHostControlService.hpp"

#include <memory>

namespace cru::ui::controls {
template <typename TControl>
class TextControlService;

class TextBox : public NoChildControl,
                public virtual IBorderControl,
                public virtual ITextHostControl {
 public:
  static constexpr std::u16string_view control_type = u"TextBox";

  static TextBox* Create() { return new TextBox(); }

 protected:
  TextBox();

 public:
  CRU_DELETE_COPY(TextBox)
  CRU_DELETE_MOVE(TextBox)

  ~TextBox() override;

  std::u16string_view GetControlType() const final { return control_type; }

  render::RenderObject* GetRenderObject() const override;

  gsl::not_null<render::TextRenderObject*> GetTextRenderObject() override;
  render::ScrollRenderObject* GetScrollRenderObject() override;

  void ApplyBorderStyle(const style::ApplyBorderStyleInfo& style) override;

 private:
  std::unique_ptr<render::BorderRenderObject> border_render_object_;
  std::unique_ptr<render::ScrollRenderObject> scroll_render_object_;
  std::unique_ptr<render::TextRenderObject> text_render_object_;

  std::unique_ptr<TextHostControlService> service_;
};
}  // namespace cru::ui::controls
