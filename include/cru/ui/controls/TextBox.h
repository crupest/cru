#pragma once
#include "NoChildControl.h"

#include "../render/BorderRenderObject.h"
#include "../render/TextRenderObject.h"
#include "IBorderControl.h"
#include "TextHostControlService.h"

#include <memory>

namespace cru::ui::controls {
class CRU_UI_API TextBox : public NoChildControl,
                           public virtual IBorderControl,
                           public virtual ITextHostControl {
 public:
  static constexpr StringView control_type = u"TextBox";

  TextBox();
  CRU_DELETE_COPY(TextBox)
  CRU_DELETE_MOVE(TextBox)
  ~TextBox() override;

  String GetControlType() const final { return control_type.ToString(); }

  render::RenderObject* GetRenderObject() const override;

  gsl::not_null<render::TextRenderObject*> GetTextRenderObject() override;
  render::ScrollRenderObject* GetScrollRenderObject() override;

  bool GetMultiLine() const;
  void SetMultiLine(bool value);

  void ApplyBorderStyle(const style::ApplyBorderStyleInfo& style) override;

 private:
  std::unique_ptr<render::BorderRenderObject> border_render_object_;
  std::unique_ptr<render::ScrollRenderObject> scroll_render_object_;
  std::unique_ptr<render::TextRenderObject> text_render_object_;

  std::unique_ptr<TextHostControlService> service_;
};
}  // namespace cru::ui::controls
