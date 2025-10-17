#include "cru/platform/graphics/cairo/PangoTextLayout.h"
#include "cru/platform/Check.h"
#include "cru/platform/GraphicsBase.h"
#include "cru/platform/graphics/Base.h"
#include "cru/platform/graphics/cairo/CairoGraphicsFactory.h"
#include "cru/platform/graphics/cairo/PangoFont.h"

#include <pango/pangocairo.h>

namespace cru::platform::graphics::cairo {
namespace {
Rect ConvertFromPango(const Rect& rect) {
  auto result = rect;
  result.left /= PANGO_SCALE;
  result.top /= PANGO_SCALE;
  result.width /= PANGO_SCALE;
  result.height /= PANGO_SCALE;
  return result;
}
}  // namespace

PangoTextLayout::PangoTextLayout(CairoGraphicsFactory* factory,
                                 std::shared_ptr<IFont> font)
    : CairoResource(factory) {
  Expects(font);
  font_ = CheckPlatform<PangoFont>(font, GetPlatformId());
  pango_layout_ = pango_cairo_create_layout(factory->GetDefaultCairo());
  pango_layout_set_font_description(pango_layout_,
                                    font_->GetPangoFontDescription());
};

PangoTextLayout::~PangoTextLayout() { g_object_unref(pango_layout_); }

std::string PangoTextLayout::GetText() { return text_; }

void PangoTextLayout::SetText(std::string new_text) {
  text_ = std::move(new_text);
  pango_layout_set_text(pango_layout_, text_.c_str(), text_.size());
}

std::shared_ptr<IFont> PangoTextLayout::GetFont() { return font_; }

void PangoTextLayout::SetFont(std::shared_ptr<IFont> font) {
  Expects(font);
  font_ = CheckPlatform<PangoFont>(font, GetPlatformId());
  pango_layout_set_font_description(pango_layout_,
                                    font_->GetPangoFontDescription());
}

void PangoTextLayout::SetMaxWidth(float max_width) {
  return pango_layout_set_width(pango_layout_, max_width * PANGO_SCALE);
}

void PangoTextLayout::SetMaxHeight(float max_height) {
  return pango_layout_set_height(pango_layout_, max_height * PANGO_SCALE);
}

bool PangoTextLayout::IsEditMode() { return edit_mode_; }

void PangoTextLayout::SetEditMode(bool enable) { edit_mode_ = enable; }

Index PangoTextLayout::GetLineIndexFromCharIndex(Index char_index) {
  int line;
  pango_layout_index_to_line_x(pango_layout_, char_index, false, &line,
                               nullptr);
  return line;
}

Index PangoTextLayout::GetLineCount() {
  return pango_layout_get_line_count(pango_layout_);
}

float PangoTextLayout::GetLineHeight(Index line_index) {
  auto line = pango_layout_get_line_readonly(pango_layout_, line_index);
  int height;
  pango_layout_line_get_height(line, &height);
  return static_cast<float>(height) / PANGO_SCALE;
}

Rect PangoTextLayout::GetTextBounds(bool includingTrailingSpace) {
  PangoRectangle rectangle;
  pango_layout_get_extents(pango_layout_, nullptr, &rectangle);
  return ConvertFromPango(
      Rect(rectangle.x, rectangle.y, rectangle.width, rectangle.height));
}

std::vector<Rect> PangoTextLayout::TextRangeRect(const TextRange& text_range) {
  auto tr = text_range.Normalize();
  auto start_index = tr.GetStart();
  auto end_index = tr.GetEnd();
  PangoRectangle rectangle;

  int start_line_index, end_line_index, start_x_pos, end_x_pos;
  pango_layout_index_to_line_x(pango_layout_, start_index, false,
                               &start_line_index, &start_x_pos);
  pango_layout_index_to_line_x(pango_layout_, end_index, false, &end_line_index,
                               &end_x_pos);

  pango_layout_index_to_pos(pango_layout_, start_index, &rectangle);
  auto top = rectangle.y;

  if (start_line_index == end_line_index) {
    auto line = pango_layout_get_line(pango_layout_, start_line_index);
    pango_layout_line_get_extents(line, nullptr, &rectangle);
    auto rect =
        ConvertFromPango(Rect(rectangle.x + start_x_pos, top,
                              end_x_pos - start_x_pos, rectangle.height));
    return {rect};
  } else {
    std::vector<Rect> result;

    auto start_line = pango_layout_get_line(pango_layout_, start_line_index);
    pango_layout_line_get_extents(start_line, nullptr, &rectangle);
    result.push_back(Rect(rectangle.x + start_x_pos, top,
                          rectangle.width - start_x_pos, rectangle.height));
    top += rectangle.height;

    for (int line_index = start_line_index + 1; line_index < end_line_index;
         line_index++) {
      auto line = pango_layout_get_line(pango_layout_, line_index);
      pango_layout_line_get_extents(line, nullptr, &rectangle);
      result.push_back(
          Rect(rectangle.x, top, rectangle.width, rectangle.height));
      top += rectangle.height;
    }

    auto end_line = pango_layout_get_line(pango_layout_, end_line_index);
    pango_layout_line_get_extents(end_line, nullptr, &rectangle);
    result.push_back(Rect(rectangle.x, top, end_x_pos, rectangle.height));

    for (auto& rect : result) {
      rect = ConvertFromPango(rect);
    }

    return result;
  }
}

Rect PangoTextLayout::TextSinglePoint(Index position, bool trailing) {
  int line_index, x_pos, y_pos = 0;
  pango_layout_index_to_line_x(pango_layout_, position, trailing, &line_index,
                               &x_pos);

  for (int i = 0; i < line_index; i++) {
    auto line = pango_layout_get_line(pango_layout_, line_index);
    PangoRectangle rectangle;
    pango_layout_line_get_extents(line, nullptr, &rectangle);
    y_pos += rectangle.height;
  }

  auto line = pango_layout_get_line(pango_layout_, line_index);
  PangoRectangle rectangle;
  pango_layout_line_get_extents(line, nullptr, &rectangle);

  return ConvertFromPango(
      Rect(rectangle.x + x_pos, y_pos, 0, rectangle.height));
}

TextHitTestResult PangoTextLayout::HitTest(const Point& point) {
  int index, trailing;
  auto inside_text =
      pango_layout_xy_to_index(pango_layout_, point.x * PANGO_SCALE,
                               point.y * PANGO_SCALE, &index, &trailing);
  return TextHitTestResult{index, trailing != 0, inside_text != 0};
}

}  // namespace cru::platform::graphics::cairo
