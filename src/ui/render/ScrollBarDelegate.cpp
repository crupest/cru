#include "cru/ui/render/ScrollBarDelegate.hpp"

#include "../Helper.hpp"
#include "cru/common/Base.hpp"
#include "cru/platform/GraphBase.hpp"
#include "cru/platform/graphics/Factory.hpp"
#include "cru/platform/graphics/Painter.hpp"
#include "cru/ui/render/ScrollRenderObject.hpp"

#include <gsl/pointers>
#include <optional>

namespace cru::ui::render {
constexpr float kScrollBarCollapseThumbWidth = 2;

ScrollBar::ScrollBar(gsl::not_null<ScrollRenderObject*> render_object)
    : render_object_(render_object) {
  // TODO: Use theme resource and delete this.
  auto collapse_thumb_brush = GetUiApplication()
                                  ->GetInstance()
                                  ->GetGraphFactory()
                                  ->CreateSolidColorBrush();
  collapse_thumb_brush->SetColor(colors::gray.WithAlpha(128));
  collapse_thumb_brush_ = std::move(collapse_thumb_brush);
}

void ScrollBar::SetEnabled(bool value) {
  CRU_UNUSED(value)
  // TODO: Implement this.
}

void ScrollBar::Draw(platform::graphics::IPainter* painter) {
  if (is_enabled_) {
    OnDraw(painter, is_expanded_);
  }
}

void ScrollBar::InstallHandlers(controls::Control* control) {
  CRU_UNUSED(control);
  // TODO: Implement this.
}

gsl::not_null<std::shared_ptr<platform::graphics::IBrush>>
ScrollBar::GetCollapseThumbBrush() const {
  // TODO: Read theme resource.
  return collapse_thumb_brush_;
}

HorizontalScrollBar::HorizontalScrollBar(
    gsl::not_null<ScrollRenderObject*> render_object)
    : ScrollBar(render_object) {}

std::optional<ScrollBarAreaKind> HorizontalScrollBar::HitTest(
    const Point& point) {
  // TODO: Implement this.
  CRU_UNUSED(point);
  return std::nullopt;
}

void HorizontalScrollBar::OnDraw(platform::graphics::IPainter* painter,
                                 bool expand) {
  const auto child = render_object_->GetFirstChild();
  if (child == nullptr) return;

  const auto view_rect = render_object_->GetViewRect();
  const auto padding_rect = render_object_->GetPaddingRect();
  const auto child_size = child->GetSize();

  if (view_rect.width >= child_size.width) return;

  const float start_percentage = view_rect.left / child_size.width;
  const float length_percentage = view_rect.width / child_size.width;
  // const float end_percentage = start_percentage + length_percentage;

  if (expand) {
    // TODO: Implement this.
  } else {
    Rect thumb_rect{padding_rect.left + padding_rect.width * start_percentage,
                    padding_rect.GetBottom() - kScrollBarCollapseThumbWidth,
                    padding_rect.width * length_percentage,
                    kScrollBarCollapseThumbWidth};
    painter->FillRectangle(thumb_rect, GetCollapseThumbBrush().get().get());
  }
}

VerticalScrollBar::VerticalScrollBar(
    gsl::not_null<ScrollRenderObject*> render_object)
    : ScrollBar(render_object) {}

std::optional<ScrollBarAreaKind> VerticalScrollBar::HitTest(
    const Point& point) {
  // TODO: Implement this.
  CRU_UNUSED(point);
  return std::nullopt;
}

void VerticalScrollBar::OnDraw(platform::graphics::IPainter* painter,
                               bool expand) {
  const auto child = render_object_->GetFirstChild();
  if (child == nullptr) return;

  const auto view_rect = render_object_->GetViewRect();
  const auto padding_rect = render_object_->GetPaddingRect();
  const auto child_size = child->GetSize();

  if (view_rect.height >= child_size.height) return;

  const float start_percentage = view_rect.top / child_size.height;
  const float length_percentage = view_rect.height / child_size.height;
  // const float end_percentage = start_percentage + length_percentage;

  if (expand) {
    // TODO: Implement this.
  } else {
    Rect thumb_rect{padding_rect.GetRight() - kScrollBarCollapseThumbWidth,
                    padding_rect.top + padding_rect.height * start_percentage,
                    kScrollBarCollapseThumbWidth,
                    padding_rect.height * length_percentage};
    painter->FillRectangle(thumb_rect, GetCollapseThumbBrush().get().get());
  }
}

ScrollBarDelegate::ScrollBarDelegate(
    gsl::not_null<ScrollRenderObject*> render_object)
    : render_object_(render_object),
      horizontal_bar_(render_object),
      vertical_bar_(render_object) {
  horizontal_bar_.ScrollAttemptEvent()->AddHandler([this](float offset) {
    this->scroll_attempt_event_.Raise({offset, 0});
  });
  vertical_bar_.ScrollAttemptEvent()->AddHandler([this](float offset) {
    this->scroll_attempt_event_.Raise({0, offset});
  });
}

void ScrollBarDelegate::DrawScrollBar(platform::graphics::IPainter* painter) {
  horizontal_bar_.Draw(painter);
  vertical_bar_.Draw(painter);
}

void ScrollBarDelegate::InstallHandlers(controls::Control* control) {
  horizontal_bar_.InstallHandlers(control);
  vertical_bar_.InstallHandlers(control);
}
}  // namespace cru::ui::render
