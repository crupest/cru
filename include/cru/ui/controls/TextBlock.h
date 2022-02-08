#pragma once
#include "NoChildControl.h"

#include "TextHostControlService.h"

namespace cru::ui::controls {
class CRU_UI_API TextBlock : public NoChildControl, public virtual ITextHostControl {
 public:
  static constexpr StringView control_type = u"TextBlock";

  static TextBlock* Create();
  static TextBlock* Create(String text, bool selectable = false);

 protected:
  TextBlock();

 public:
  TextBlock(const TextBlock& other) = delete;
  TextBlock(TextBlock&& other) = delete;
  TextBlock& operator=(const TextBlock& other) = delete;
  TextBlock& operator=(TextBlock&& other) = delete;
  ~TextBlock() override;

  String GetControlType() const final { return control_type.ToString(); }

  render::RenderObject* GetRenderObject() const override;

  String GetText() const;
  void SetText(String text);

  bool IsSelectable() const;
  void SetSelectable(bool value);

  gsl::not_null<render::TextRenderObject*> GetTextRenderObject() override;
  render::ScrollRenderObject* GetScrollRenderObject() override {
    return nullptr;
  }

 private:
  std::unique_ptr<render::TextRenderObject> text_render_object_;

  std::unique_ptr<TextHostControlService> service_;
};
}  // namespace cru::ui::controls
