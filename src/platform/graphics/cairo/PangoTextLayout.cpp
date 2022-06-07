#include "cru/platform/graphics/cairo/PangoTextLayout.h"
#include "cru/common/StringUtil.h"
#include "cru/platform/Check.h"
#include "cru/platform/graphics/cairo/CairoGraphicsFactory.h"
#include "cru/platform/graphics/cairo/PangoFont.h"

namespace cru::platform::graphics::cairo {
PangoTextLayout::PangoTextLayout(CairoGraphicsFactory* factory,
                                 std::shared_ptr<IFont> font)
    : CairoResource(factory) {
  Expects(font);
  font_ = CheckPlatform<PangoFont>(font, GetPlatformId());
  pango_layout_ = pango_layout_new(factory->GetDefaultPangoContext());
  pango_layout_set_font_description(pango_layout_,
                                    font_->GetPangoFontDescription());
};

PangoTextLayout::~PangoTextLayout() { g_object_unref(pango_layout_); }

String PangoTextLayout::GetText() { return text_; }

void PangoTextLayout::SetText(String new_text) {
  text_ = std::move(new_text);
  utf8_text_ = text_.ToUtf8();
  pango_layout_set_text(pango_layout_, utf8_text_.c_str(), utf8_text_.size());
}

std::shared_ptr<IFont> PangoTextLayout::GetFont() { return font_; }

void PangoTextLayout::SetFont(std::shared_ptr<IFont> font) {
  Expects(font);
  font_ = CheckPlatform<PangoFont>(font, GetPlatformId());
  pango_layout_set_font_description(pango_layout_,
                                    font_->GetPangoFontDescription());
}

void PangoTextLayout::SetMaxWidth(float max_width) {
  return pango_layout_set_width(pango_layout_, max_width);
}

void PangoTextLayout::SetMaxHeight(float max_height) {
  return pango_layout_set_height(pango_layout_, max_height);
}

bool PangoTextLayout::IsEditMode() { return edit_mode_; }

void PangoTextLayout::SetEditMode(bool enable) { edit_mode_ = enable; }

Index PangoTextLayout::GetLineIndexFromCharIndex(Index char_index) {
  int line;
  pango_layout_index_to_line_x(pango_layout_,
                               FromUtf16IndexToUtf8Index(char_index), false,
                               &line, nullptr);
  return line;
}

Index PangoTextLayout::GetLineCount() {
  return pango_layout_get_line_count(pango_layout_);
}

float PangoTextLayout::GetLineHeight(Index line_index) {
  auto line = pango_layout_get_line_readonly(pango_layout_, line_index);
  int height;
  pango_layout_line_get_height(line, &height);
  return height;
}

Index PangoTextLayout::FromUtf8IndexToUtf16Index(Index index) {
  Utf8CodePointIterator iter(utf8_text_.data(), utf8_text_.size());
  int cp_count = 0;
  while ((!iter.IsPastEnd()) && iter.GetPosition() < index) {
    ++iter;
    cp_count++;
  }
  return text_.IndexFromCodePointToCodeUnit(cp_count);
}

Index PangoTextLayout::FromUtf16IndexToUtf8Index(Index index) {
  Index cp_index = text_.IndexFromCodeUnitToCodePoint(index);
  Utf8CodePointIterator iter(utf8_text_.data(), utf8_text_.size());

  for (Index i = 0; i < cp_index; ++i) {
    ++iter;
  }

  return iter.GetPosition();
}

}  // namespace cru::platform::graphics::cairo
