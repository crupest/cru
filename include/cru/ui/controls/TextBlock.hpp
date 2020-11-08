#pragma once
#include "NoChildControl.hpp"

namespace cru::ui::controls {
template <typename TControl>
class TextControlService;

class TextBlock : public NoChildControl {
 public:
  static constexpr std::u16string_view control_type = u"TextBlock";

  static TextBlock* Create() { return new TextBlock(); }

 protected:
  TextBlock();

 public:
  TextBlock(const TextBlock& other) = delete;
  TextBlock(TextBlock&& other) = delete;
  TextBlock& operator=(const TextBlock& other) = delete;
  TextBlock& operator=(TextBlock&& other) = delete;
  ~TextBlock() override;

  std::u16string_view GetControlType() const final { return control_type; }

  render::RenderObject* GetRenderObject() const override;

  std::u16string GetText() const;
  void SetText(std::u16string text);

  gsl::not_null<render::TextRenderObject*> GetTextRenderObject();
  render::ScrollRenderObject* GetScrollRenderObject() { return nullptr; }

 private:
  std::unique_ptr<render::TextRenderObject> text_render_object_;

  std::unique_ptr<TextControlService<TextBlock>> service_;
};
}  // namespace cru::ui::controls
