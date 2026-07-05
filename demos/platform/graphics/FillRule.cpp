#include "Base.h"
#include "cru/platform/Color.h"
#include "cru/platform/Matrix.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/graphics/Painter.h"

#include <memory>

int main() {
  CruPlatformGraphicsDemo demo("fill-rule-demo.png", 400, 200);

  auto factory = demo.GetFactory();
  auto painter = demo.GetPainter();

  // Clear background so the EvenOdd hole is visible.
  painter->Clear(cru::platform::colors::lightgray);

  // Build geometry: outer rectangle wrapping inner rectangle.
  // Both sub-paths go clockwise so NonZero fills both; EvenOdd punches a hole.
  auto build_rects = [](auto* builder) {
    builder->MoveTo({25, 25});
    builder->LineTo({175, 25});
    builder->LineTo({175, 175});
    builder->LineTo({25, 175});
    builder->CloseFigure(true);
    builder->MoveTo({50, 50});
    builder->LineTo({150, 50});
    builder->LineTo({150, 150});
    builder->LineTo({50, 150});
    builder->CloseFigure(true);
  };

  auto builder_even_odd = factory->CreateGeometryBuilder();
  builder_even_odd->SetFillRule(cru::platform::graphics::GeometryFillRule::EvenOdd);
  build_rects(builder_even_odd.get());
  auto geo_even_odd = builder_even_odd->Build();

  auto builder_non_zero = factory->CreateGeometryBuilder();
  builder_non_zero->SetFillRule(cru::platform::graphics::GeometryFillRule::NonZero);
  build_rects(builder_non_zero.get());
  auto geo_non_zero = builder_non_zero->Build();

  auto brush = factory->CreateSolidColorBrush(cru::platform::colors::skyblue);

  // Left: EvenOdd — inner rectangle appears as a hole.
  painter->FillGeometry(geo_even_odd.get(), brush.get());

  // Right: NonZero — inner rectangle is filled solid.
  painter->PushState();
  painter->ConcatTransform(cru::platform::Matrix::Translation(200, 0));
  painter->FillGeometry(geo_non_zero.get(), brush.get());
  painter->PopState();

  return 0;
}
