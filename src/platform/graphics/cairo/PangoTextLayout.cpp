#include "cru/platform/graphics/cairo/PangoTextLayout.h"
#include "cru/base/StringUtil.h"
#include "cru/platform/Check.h"
#include "cru/platform/GraphicsBase.h"
#include "cru/platform/graphics/Base.h"
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

Rect PangoTextLayout::GetTextBounds(bool includingTrailingSpace) {
  PangoRectangle rectangle;
  pango_layout_get_extents(pango_layout_, nullptr, &rectangle);
  return Rect(rectangle.x, rectangle.y, rectangle.width, rectangle.height);
}

std::vector<Rect> PangoTextLayout::TextRangeRect(const TextRange& text_range) {
  auto tr = text_range.Normalize();
  auto utf8_start_index = FromUtf16IndexToUtf8Index(tr.GetStart());
  auto utf8_end_index = FromUtf16IndexToUtf8Index(tr.GetEnd());

  int start_line_index, end_line_index, start_x_pos, end_x_pos;
  pango_layout_index_to_line_x(pango_layout_, utf8_start_index, false,
                               &start_line_index, &start_x_pos);
  pango_layout_index_to_line_x(pango_layout_, utf8_end_index, false,
                               &end_line_index, &end_x_pos);

  if (start_line_index == end_line_index) {
    auto line = pango_layout_get_line(pango_layout_, start_line_index);
    PangoRectangle rectangle;
    pango_layout_line_get_extents(line, nullptr, &rectangle);
    return {Rect(rectangle.x + start_x_pos, rectangle.y,
                 end_x_pos - start_x_pos, rectangle.height)};
  } else {
    std::vector<Rect> result;

    PangoRectangle rectangle;

    auto start_line = pango_layout_get_line(pango_layout_, start_line_index);
    pango_layout_line_get_extents(start_line, nullptr, &rectangle);
    result.push_back(Rect(rectangle.x + start_x_pos, rectangle.y,
                          rectangle.width - start_x_pos, rectangle.height));

    for (int line_index = start_line_index + 1; line_index < end_line_index;
         line_index++) {
      auto line = pango_layout_get_line(pango_layout_, line_index);
      pango_layout_line_get_extents(line, nullptr, &rectangle);
      result.push_back(
          Rect(rectangle.x, rectangle.y, rectangle.width, rectangle.height));
    }

    auto end_line = pango_layout_get_line(pango_layout_, end_line_index);
    pango_layout_line_get_extents(end_line, nullptr, &rectangle);
    result.push_back(
        Rect(rectangle.x, rectangle.y, end_x_pos, rectangle.height));

    return result;
  }
}

Rect PangoTextLayout::TextSinglePoint(Index position, bool trailing) {
  auto utf8_index = FromUtf16IndexToUtf8Index(position);
  int line_index, x_pos;
  pango_layout_index_to_line_x(pango_layout_, utf8_index, trailing, &line_index,
                               &x_pos);

  auto line = pango_layout_get_line(pango_layout_, line_index);
  PangoRectangle rectangle;
  pango_layout_line_get_extents(line, nullptr, &rectangle);

  return Rect(rectangle.x + x_pos, rectangle.y, 0, rectangle.height);
}

TextHitTestResult PangoTextLayout::HitTest(const Point& point) {
  int index, trailing;
  auto inside_text = pango_layout_xy_to_index(pango_layout_, point.x, point.y,
                                              &index, &trailing);
  return TextHitTestResult{FromUtf8IndexToUtf16Index(index), trailing != 0,
                           inside_text != 0};
}

}  // namespace cru::platform::graphics::cairo
