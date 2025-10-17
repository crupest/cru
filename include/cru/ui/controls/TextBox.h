#pragma once
#include "NoChildControl.h"

#include "../render/BorderRenderObject.h"
#include "../render/TextRenderObject.h"
#include "IBorderControl.h"
#include "IContentBrushControl.h"
#include "IFontControl.h"
#include "TextHostControlService.h"
#include "cru/platform/graphics/Brush.h"

#include <memory>

namespace cru::ui::controls {
class CRU_UI_API TextBox : public NoChildControl,
                           public virtual IBorderControl,
                           public virtual ITextHostControl,
                           public virtual IContentBrushControl,
                           public virtual IFontControl {
 public:
  static constexpr std::string_view control_type = "TextBox";

  TextBox();
  CRU_DELETE_COPY(TextBox)
  CRU_DELETE_MOVE(TextBox)
  ~TextBox() override;

  std::string GetControlType() const final { return std::string(control_type); }

  render::RenderObject* GetRenderObject() const override;

  render::TextRenderObject* GetTextRenderObject() override;
  render::ScrollRenderObject* GetScrollRenderObject() override;

  bool GetMultiLine() const;
  void SetMultiLine(bool value);

  void ApplyBorderStyle(const style::ApplyBorderStyleInfo& style) override;

  std::string GetText() const { return service_->GetText(); }
  std::string_view GetTextView() const { return service_->GetTextView(); }
  void SetText(std::string text) { service_->SetText(std::move(text)); }

  IEvent<std::nullptr_t>* TextChangeEvent() {
    return service_->TextChangeEvent();
  }

  std::shared_ptr<platform::graphics::IFont> GetFont() const override {
    return text_render_object_->GetFont();
  }
  void SetFont(std::shared_ptr<platform::graphics::IFont> font) override {
    text_render_object_->SetFont(std::move(font));
  }

  std::shared_ptr<platform::graphics::IBrush> GetTextBrush() const {
    return text_render_object_->GetBrush();
  }
  void SetTextBrush(std::shared_ptr<platform::graphics::IBrush> brush) {
    text_render_object_->SetBrush(std::move(brush));
  }

  std::shared_ptr<platform::graphics::IBrush> GetContentBrush() const override {
    return GetTextBrush();
  }

  void SetContentBrush(
      std::shared_ptr<platform::graphics::IBrush> brush) override {
    SetTextBrush(std::move(brush));
  }

 private:
  std::unique_ptr<render::BorderRenderObject> border_render_object_;
  std::unique_ptr<render::ScrollRenderObject> scroll_render_object_;
  std::unique_ptr<render::TextRenderObject> text_render_object_;

  std::unique_ptr<TextHostControlService> service_;
};
}  // namespace cru::ui::controls
