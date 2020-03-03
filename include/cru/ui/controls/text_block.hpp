#pragma once
#include "../no_child_control.hpp"

#include "text_common.hpp"

#include <memory>

namespace cru::ui::render {
class StackLayoutRenderObject;
class TextRenderObject;
class CanvasRenderObject;
}  // namespace cru::ui::render

namespace cru::ui::controls {
class TextBlock : public NoChildControl, public virtual ITextControl {
 public:
  static constexpr std::string_view control_type = "TextBlock";

  static TextBlock* Create() { return new TextBlock(); }

 protected:
  TextBlock();

 public:
  TextBlock(const TextBlock& other) = delete;
  TextBlock(TextBlock&& other) = delete;
  TextBlock& operator=(const TextBlock& other) = delete;
  TextBlock& operator=(TextBlock&& other) = delete;
  ~TextBlock() override;

  std::string_view GetControlType() const final { return control_type; }

  render::RenderObject* GetRenderObject() const override;

  std::string GetText() const;
  void SetText(std::string text);

  render::TextRenderObject* GetTextRenderObject() override;
  render::CanvasRenderObject* GetCaretRenderObject() override;
  platform::graph::IBrush* GetCaretBrush() override;

 private:
  std::unique_ptr<render::StackLayoutRenderObject> root_render_object_;
  std::unique_ptr<render::TextRenderObject> text_render_object_;
  std::unique_ptr<render::CanvasRenderObject> caret_render_object_;
  std::shared_ptr<platform::graph::IBrush> caret_brush_;

  std::unique_ptr<TextControlService> service_;
};
}  // namespace cru::ui::controls
