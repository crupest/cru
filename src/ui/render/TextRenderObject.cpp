#include "cru/ui/render/TextRenderObject.h"

#include "cru/base/log/Logger.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/graphics/Painter.h"
#include "cru/platform/graphics/TextLayout.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/ui/DebugFlags.h"
#include "cru/ui/render/RenderObject.h"

#include <algorithm>
#include <limits>

namespace cru::ui::render {
TextRenderObject::TextRenderObject(
    std::shared_ptr<platform::graphics::IBrush> brush,
    std::shared_ptr<platform::graphics::IFont> font,
    std::shared_ptr<platform::graphics::IBrush> selection_brush,
    std::shared_ptr<platform::graphics::IBrush> caret_brush)
    : RenderObject(kRenderObjectName) {
  Expects(brush);
  Expects(font);
  Expects(selection_brush);
  Expects(caret_brush);

  brush.swap(brush_);
  font.swap(font_);
  selection_brush.swap(selection_brush_);
  caret_brush.swap(caret_brush_);

  const auto graph_factory =
      platform::gui::IUiApplication::GetInstance()->GetGraphicsFactory();
  text_layout_ = graph_factory->CreateTextLayout(font_, "");
}

std::string TextRenderObject::GetText() { return text_layout_->GetText(); }

void TextRenderObject::SetText(std::string new_text) {
  text_layout_->SetText(std::move(new_text));
  InvalidateLayout();
}

void TextRenderObject::SetBrush(
    std::shared_ptr<platform::graphics::IBrush> new_brush) {
  Expects(new_brush);
  new_brush.swap(brush_);
  InvalidatePaint();
}

std::shared_ptr<platform::graphics::IFont> TextRenderObject::GetFont() {
  return text_layout_->GetFont();
}

void TextRenderObject::SetFont(
    std::shared_ptr<platform::graphics::IFont> font) {
  Expects(font);
  text_layout_->SetFont(std::move(font));
}

bool TextRenderObject::IsEditMode() { return text_layout_->IsEditMode(); }

void TextRenderObject::SetEditMode(bool enable) {
  text_layout_->SetEditMode(enable);
}

Index TextRenderObject::GetLineCount() { return text_layout_->GetLineCount(); }

Index TextRenderObject::GetLineIndexFromCharIndex(Index char_index) {
  return text_layout_->GetLineIndexFromCharIndex(char_index);
}

float TextRenderObject::GetLineHeight(Index line_index) {
  return text_layout_->GetLineHeight(line_index);
}

std::vector<Rect> TextRenderObject::TextRangeRect(const TextRange& text_range) {
  return text_layout_->TextRangeRect(text_range);
}

Rect TextRenderObject::TextSinglePoint(Index position, bool trailing) {
  return text_layout_->TextSinglePoint(position, trailing);
}

platform::graphics::TextHitTestResult TextRenderObject::TextHitTest(
    const Point& point) {
  return text_layout_->HitTest(point);
}

void TextRenderObject::SetSelectionRange(std::optional<TextRange> new_range) {
  selection_range_ = std::move(new_range);
  InvalidatePaint();
}

void TextRenderObject::SetSelectionBrush(
    std::shared_ptr<platform::graphics::IBrush> new_brush) {
  Expects(new_brush);
  new_brush.swap(selection_brush_);
  if (selection_range_ && selection_range_->count) {
    InvalidatePaint();
  }
}

void TextRenderObject::SetDrawCaret(bool draw_caret) {
  if (draw_caret_ != draw_caret) {
    draw_caret_ = draw_caret;
    InvalidatePaint();
  }
}

void TextRenderObject::SetCaretPosition(Index position) {
  if (position != caret_position_) {
    caret_position_ = position;
    if (draw_caret_) {
      InvalidatePaint();
    }
  }
}

void TextRenderObject::GetCaretBrush(
    std::shared_ptr<platform::graphics::IBrush> brush) {
  Expects(brush);
  brush.swap(caret_brush_);
  if (draw_caret_) {
    InvalidatePaint();
  }
}

void TextRenderObject::SetCaretWidth(const float width) {
  Expects(width >= 0.0f);

  caret_width_ = width;
  if (draw_caret_) {
    InvalidatePaint();
  }
}

Rect TextRenderObject::GetCaretRectInContent() {
  auto caret_pos = this->caret_position_;
  Index text_size = this->GetText().size();
  if (caret_pos < 0) {
    caret_pos = 0;
  } else if (caret_pos > text_size) {
    caret_pos = text_size;
  }

  const auto caret_top_center =
      this->text_layout_->TextSinglePoint(caret_pos, false);

  const auto caret_width = this->caret_width_;

  auto rect = Rect{caret_top_center.left - caret_width / 2.0f,
                   caret_top_center.top, caret_width, caret_top_center.height};

  return rect;
}

Rect TextRenderObject::GetCaretRect() {
  auto rect = GetCaretRectInContent();
  const auto content_rect = GetContentRect();

  rect.left += content_rect.left;
  rect.top += content_rect.top;

  return rect;
}

void TextRenderObject::SetMeasureIncludingTrailingSpace(bool including) {
  if (is_measure_including_trailing_space_ == including) return;
  is_measure_including_trailing_space_ = including;
  InvalidateLayout();
}

RenderObject* TextRenderObject::HitTest(const Point& point) {
  const auto padding_rect = GetPaddingRect();
  return padding_rect.IsPointInside(point) ? this : nullptr;
}

void TextRenderObject::Draw(platform::graphics::IPainter* painter) {
  if constexpr (debug_flags::draw) {
    CRU_LOG_TAG_DEBUG(
        "Begin to paint, total_offset: {}, size: {}, text_layout: "
        "{}, brush: {}.",
        this->GetTotalOffset(), this->GetDesiredSize(),
        this->text_layout_->GetDebugString(), this->brush_->GetDebugString());
  }

  if (this->selection_range_.has_value()) {
    const auto&& rects = text_layout_->TextRangeRect(*this->selection_range_);
    for (const auto& rect : rects)
      painter->FillRectangle(rect, this->GetSelectionBrush().get());
  }

  painter->DrawText(Point{}, text_layout_.get(), brush_.get());

  if (this->draw_caret_ && this->caret_width_ != 0.0f) {
    painter->FillRectangle(GetCaretRectInContent(), this->caret_brush_.get());
  }
}

Size TextRenderObject::OnMeasureContent(const MeasureRequirement& requirement,
                                        const MeasureSize& preferred_size) {
  float measure_width;
  if (preferred_size.width.IsSpecified())
    measure_width = preferred_size.width.GetLengthOrUndefined();
  else
    measure_width = requirement.max.width.GetLengthOrMax();

  text_layout_->SetMaxWidth(measure_width);
  text_layout_->SetMaxHeight(std::numeric_limits<float>::max());

  const Size text_size(
      text_layout_->GetTextBounds(is_measure_including_trailing_space_)
          .GetRightBottom());
  auto result = text_size;

  result.width = std::max(result.width, preferred_size.width.GetLengthOr0());
  result.width = std::max(result.width, requirement.min.width.GetLengthOr0());
  result.width = std::min(result.width, requirement.max.width.GetLengthOrMax());

  result.height = std::max(result.height, preferred_size.height.GetLengthOr0());
  result.height =
      std::max(result.height, requirement.min.height.GetLengthOr0());
  result.height =
      std::min(result.height, requirement.max.height.GetLengthOrMax());

  return result;
}

void TextRenderObject::OnLayoutContent(const Rect& content_rect) {
  CRU_UNUSED(content_rect)
}
}  // namespace cru::ui::render
