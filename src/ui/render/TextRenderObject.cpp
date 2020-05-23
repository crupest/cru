#include "cru/ui/render/TextRenderObject.hpp"

#include "../Helper.hpp"
#include "cru/platform/graph/Factory.hpp"
#include "cru/platform/graph/TextLayout.hpp"
#include "cru/platform/graph/util/Painter.hpp"

#include <algorithm>

namespace cru::ui::render {
TextRenderObject::TextRenderObject(
    std::shared_ptr<platform::graph::IBrush> brush,
    std::shared_ptr<platform::graph::IFont> font,
    std::shared_ptr<platform::graph::IBrush> selection_brush,
    std::shared_ptr<platform::graph::IBrush> caret_brush) {
  Expects(brush);
  Expects(font);
  Expects(selection_brush);
  Expects(caret_brush);

  SetChildMode(ChildMode::None);

  brush.swap(brush_);
  font.swap(font_);
  selection_brush.swap(selection_brush_);
  caret_brush.swap(caret_brush_);

  const auto graph_factory = GetGraphFactory();
  text_layout_ = graph_factory->CreateTextLayout(font_, "");
}

TextRenderObject::~TextRenderObject() = default;

std::string TextRenderObject::GetText() const {
  return text_layout_->GetText();
}

void TextRenderObject::SetText(std::string new_text) {
  text_layout_->SetText(std::move(new_text));
}

void TextRenderObject::SetBrush(
    std::shared_ptr<platform::graph::IBrush> new_brush) {
  Expects(new_brush);
  new_brush.swap(brush_);
  InvalidatePaint();
}

std::shared_ptr<platform::graph::IFont> TextRenderObject::GetFont() const {
  return text_layout_->GetFont();
}

void TextRenderObject::SetFont(std::shared_ptr<platform::graph::IFont> font) {
  Expects(font);
  text_layout_->SetFont(std::move(font));
}

std::vector<Rect> TextRenderObject::TextRangeRect(const TextRange& text_range) {
  return text_layout_->TextRangeRect(text_range);
}

Point TextRenderObject::TextSinglePoint(gsl::index position, bool trailing) {
  return text_layout_->TextSinglePoint(position, trailing);
}

platform::graph::TextHitTestResult TextRenderObject::TextHitTest(
    const Point& point) {
  return text_layout_->HitTest(point);
}

void TextRenderObject::SetSelectionRange(std::optional<TextRange> new_range) {
  selection_range_ = std::move(new_range);
  InvalidatePaint();
}

void TextRenderObject::SetSelectionBrush(
    std::shared_ptr<platform::graph::IBrush> new_brush) {
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

void TextRenderObject::SetCaretPosition(gsl::index position) {
  if (position != caret_position_) {
    caret_position_ = position;
    if (draw_caret_) {
      InvalidatePaint();
    }
  }
}

void TextRenderObject::GetCaretBrush(
    std::shared_ptr<platform::graph::IBrush> brush) {
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

void TextRenderObject::Draw(platform::graph::IPainter* painter) {
  platform::graph::util::WithTransform(
      painter,
      platform::Matrix::Translation(GetMargin().left + GetPadding().left,
                                    GetMargin().top + GetPadding().top),
      [this](platform::graph::IPainter* p) {
        if (this->selection_range_.has_value()) {
          const auto&& rects =
              text_layout_->TextRangeRect(this->selection_range_.value());
          for (const auto& rect : rects)
            p->FillRectangle(rect, this->GetSelectionBrush().get());
        }

        p->DrawText(Point{}, text_layout_.get(), brush_.get());

        if (this->draw_caret_ && this->caret_width_ != 0.0f) {
          auto caret_pos = this->caret_position_;
          gsl::index text_size = this->GetText().size();
          if (caret_pos < 0) {
            caret_pos = 0;
          } else if (caret_pos > text_size) {
            caret_pos = text_size;
          }

          const auto caret_top_center =
              this->text_layout_->TextSinglePoint(caret_pos, false);

          const auto font_height = this->font_->GetFontSize();
          const auto caret_width = this->caret_width_;

          p->FillRectangle(Rect{caret_top_center.x - caret_width / 2.0f,
                                caret_top_center.y, caret_width, font_height},
                           this->caret_brush_.get());
        }
      });
}

RenderObject* TextRenderObject::HitTest(const Point& point) {
  const auto padding_rect = GetPaddingRect();
  return padding_rect.IsPointInside(point) ? this : nullptr;
}

Size TextRenderObject::OnMeasureContent(const Size& available_size) {
  text_layout_->SetMaxWidth(available_size.width);
  text_layout_->SetMaxHeight(available_size.height);
  return text_layout_->GetTextBounds().GetSize();
}

void TextRenderObject::OnLayoutContent(const Rect& content_rect) {
  CRU_UNUSED(content_rect)
}

void TextRenderObject::OnAfterLayout() {
  const auto&& size = GetContentRect().GetSize();
  text_layout_->SetMaxWidth(size.width);
  text_layout_->SetMaxHeight(size.height);
}

}  // namespace cru::ui::render