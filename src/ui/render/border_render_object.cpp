#include "border_render_object.hpp"

#include "cru_debug.hpp"

namespace cru::ui::render {
BorderRenderObject::BorderRenderObject(Microsoft::WRL::ComPtr<ID2D1Brush> brush)
    : border_brush_(std::move(brush)) {}

void BorderRenderObject::OnMeasureCore(const Size& available_size) {
  const auto margin = GetMargin();
  const auto padding = GetPadding();
  Size margin_border_padding_size{
      margin.GetHorizontalTotal() + padding.GetHorizontalTotal(),
      margin.GetVerticalTotal() + padding.GetVerticalTotal()};

  if (is_enabled_) {
    margin_border_padding_size.width += border_width_.GetHorizontalTotal();
    margin_border_padding_size.height += border_width_.GetVerticalTotal();
  }

  auto coerced_margin_border_padding_size = margin_border_padding_size;
  if (coerced_margin_border_padding_size.width > available_size.width) {
    debug::DebugMessage(
        L"Measure: horizontal length of padding, border and margin is bigger "
        L"than available length.");
    coerced_margin_border_padding_size.width = available_size.width;
  }
  if (coerced_margin_border_padding_size.height > available_size.height) {
    debug::DebugMessage(
        L"Measure: vertical length of padding, border and margin is bigger "
        L"than available length.");
    coerced_margin_border_padding_size.height = available_size.height;
  }

  const auto coerced_content_available_size =
      available_size - coerced_margin_border_padding_size;

  const auto actual_content_size =
      OnMeasureContent(coerced_content_available_size);

  SetPreferredSize(coerced_margin_border_padding_size + actual_content_size);
}

void BorderRenderObject::OnLayoutCore(const Rect& rect) {
  const auto margin = GetMargin();
  const auto padding = GetPadding();
  Size margin_border_padding_size{
      margin.GetHorizontalTotal() + padding.GetHorizontalTotal(),
      margin.GetVerticalTotal() + padding.GetVerticalTotal()};

  if (is_enabled_) {
    margin_border_padding_size.width += border_width_.GetHorizontalTotal();
    margin_border_padding_size.height += border_width_.GetVerticalTotal();
  }

  const auto content_available_size =
      rect.GetSize() - margin_border_padding_size;
  auto coerced_content_available_size = content_available_size;

  if (coerced_content_available_size.width < 0) {
    debug::DebugMessage(
        L"Layout: horizontal length of padding, border and margin is bigger "
        L"than available length.");
    coerced_content_available_size.width = 0;
  }
  if (coerced_content_available_size.height < 0) {
    debug::DebugMessage(
        L"Layout: vertical length of padding, border and margin is bigger than "
        L"available length.");
    coerced_content_available_size.height = 0;
  }

  OnLayoutContent(
      Rect{margin.left + (is_enabled_ ? border_width_.left : 0) + padding.left,
           margin.top + (is_enabled_ ? border_width_.top : 0) + padding.top,
           coerced_content_available_size.width,
           coerced_content_available_size.height});
}

Size BorderRenderObject::OnMeasureContent(const Size& available_size) {
  const auto child = GetChild();
  if (child) {
    child->Measure(available_size);
    return child->GetPreferredSize();
  } else {
    return Size::Zero();
  }
}

void BorderRenderObject::OnLayoutContent(const Rect& content_rect) {
  const auto child = GetChild();
  if (child) {
    child->Layout(content_rect);
  }
}
}  // namespace cru::ui::render
