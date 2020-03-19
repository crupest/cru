#pragma once
#include "../no_child_control.hpp"

namespace cru::ui::controls {
template <typename TControl>
class TextControlService;

class TextBlock : public NoChildControl {
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

  render::TextRenderObject* GetTextRenderObject();
  render::CanvasRenderObject* GetCaretRenderObject();
  std::shared_ptr<platform::graph::IBrush> GetCaretBrush();

 private:
  std::unique_ptr<render::StackLayoutRenderObject> root_render_object_;
  std::unique_ptr<render::TextRenderObject> text_render_object_;
  std::unique_ptr<render::CanvasRenderObject> caret_render_object_;
  std::shared_ptr<platform::graph::IBrush> caret_brush_;

  std::unique_ptr<TextControlService<TextBlock>> service_;
};
}  // namespace cru::ui::controls
