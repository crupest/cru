#pragma once
#include "Control.h"
#include "../render/TextRenderObject.h"
#include "IContentBrushControl.h"
#include "IFontControl.h"
#include "TextHostControlService.h"

#include <cru/platform/graphics/Brush.h>
#include <cru/platform/graphics/Font.h>

namespace cru::ui::controls {
class CRU_UI_API TextBlock : public Control,
                             public virtual ITextHostControl,
                             public virtual IFontControl,
                             public virtual IContentBrushControl {
 public:
  static constexpr std::string_view kControlType = "TextBlock";

  static std::unique_ptr<TextBlock> Create(std::string text,
                                           bool selectable = false) {
    auto c = std::make_unique<TextBlock>();
    c->SetText(std::move(text));
    c->SetSelectable(selectable);
    return std::move(c);
  }

 public:
  TextBlock();
  TextBlock(const TextBlock& other) = delete;
  TextBlock(TextBlock&& other) = delete;
  TextBlock& operator=(const TextBlock& other) = delete;
  TextBlock& operator=(TextBlock&& other) = delete;
  ~TextBlock() override;

  std::string GetControlType() const final { return std::string(kControlType); }

  render::RenderObject* GetRenderObject() const override;

  std::string GetText() const;
  void SetText(std::string text);

  bool IsSelectable() const;
  void SetSelectable(bool value);

  std::shared_ptr<platform::graphics::IBrush> GetTextBrush() const {
    return text_render_object_->GetBrush();
  }
  void SetTextBrush(std::shared_ptr<platform::graphics::IBrush> brush) {
    text_render_object_->SetBrush(std::move(brush));
  }
  void SetTextColor(const Color& color);

  render::TextRenderObject* GetTextRenderObject() override;
  render::ScrollRenderObject* GetScrollRenderObject() override {
    return nullptr;
  }

  std::shared_ptr<platform::graphics::IFont> GetFont() const override {
    return text_render_object_->GetFont();
  }
  void SetFont(std::shared_ptr<platform::graphics::IFont> font) override {
    text_render_object_->SetFont(std::move(font));
  }

  std::shared_ptr<platform::graphics::IBrush> GetContentBrush() const override {
    return GetTextBrush();
  }

  void SetContentBrush(
      std::shared_ptr<platform::graphics::IBrush> brush) override {
    SetTextBrush(std::move(brush));
  }

 private:
  std::unique_ptr<render::TextRenderObject> text_render_object_;

  std::unique_ptr<TextHostControlService> service_;
};
}  // namespace cru::ui::controls
