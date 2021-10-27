#include "cru/osx/graphics/quartz/Painter.hpp"

#include "cru/common/Logger.hpp"
#include "cru/osx/graphics/quartz/Brush.hpp"
#include "cru/osx/graphics/quartz/Convert.hpp"
#include "cru/osx/graphics/quartz/Geometry.hpp"
#include "cru/osx/graphics/quartz/TextLayout.hpp"
#include "cru/platform/Check.hpp"
#include "cru/platform/Color.hpp"
#include "cru/platform/Exception.hpp"

namespace cru::platform::graphics::osx::quartz {
QuartzCGContextPainter::QuartzCGContextPainter(
    IGraphicsFactory* graphics_factory, CGContextRef cg_context,
    bool auto_release, const Size& size,
    std::function<void(QuartzCGContextPainter*)> on_end_draw)
    : OsxQuartzResource(graphics_factory),
      cg_context_(cg_context),
      auto_release_(auto_release),
      size_(size),
      on_end_draw_(std::move(on_end_draw)) {
  Expects(cg_context);

  CGContextConcatCTM(cg_context_,
                     CGAffineTransformInvert(CGContextGetCTM(cg_context_)));

  transform_ = Matrix::Scale(1, -1) * Matrix::Translation(0, size.height);
  CGContextConcatCTM(cg_context_, Convert(transform_));

  // log::TagDebug(log_tag,
  //               u"Created with CGContext: {}, Auto Release: {},  Size: {}.",
  //               cg_context, auto_release, size_);
}

QuartzCGContextPainter::~QuartzCGContextPainter() {
  DoEndDraw();
  if (auto_release_) {
    CGContextRelease(cg_context_);
    cg_context_ = nullptr;
  }
}

Matrix QuartzCGContextPainter::GetTransform() { return transform_; }

void QuartzCGContextPainter::SetTransform(const Matrix& matrix) {
  CGContextConcatCTM(cg_context_, Convert(*transform_.Inverted()));
  CGContextConcatCTM(cg_context_, Convert(matrix));
  transform_ = matrix;
}

void QuartzCGContextPainter::ConcatTransform(const Matrix& matrix) {
  CGContextConcatCTM(cg_context_, Convert(matrix));
  transform_ = matrix * transform_;
}

void QuartzCGContextPainter::Clear(const Color& color) {
  Validate();

  CGContextSetRGBFillColor(cg_context_, color.GetFloatRed(),
                           color.GetFloatGreen(), color.GetFloatBlue(),
                           color.GetFloatAlpha());
  CGContextFillRect(cg_context_, Convert(Rect{Point{}, size_}));

  // log::TagDebug(log_tag, u"Clear with color {}, size {}.", color, size_);
}

void QuartzCGContextPainter::DrawLine(const Point& start, const Point& end,
                                      IBrush* brush, float width) {
  Validate();

  CGContextBeginPath(cg_context_);
  CGContextMoveToPoint(cg_context_, start.x, start.y);
  CGContextAddLineToPoint(cg_context_, end.x, end.y);

  QuartzBrush* b = CheckPlatform<QuartzBrush>(brush, GetPlatformId());
  b->Select(cg_context_);
  SetLineWidth(width);

  CGContextStrokePath(cg_context_);
}

void QuartzCGContextPainter::StrokeRectangle(const Rect& rectangle,
                                             IBrush* brush, float width) {
  Validate();

  QuartzBrush* b = CheckPlatform<QuartzBrush>(brush, GetPlatformId());
  b->Select(cg_context_);
  CGContextStrokeRectWithWidth(cg_context_, Convert(rectangle), width);
}

void QuartzCGContextPainter::FillRectangle(const Rect& rectangle,
                                           IBrush* brush) {
  Validate();

  QuartzBrush* b = CheckPlatform<QuartzBrush>(brush, GetPlatformId());
  b->Select(cg_context_);
  CGContextFillRect(cg_context_, Convert(rectangle));
}

void QuartzCGContextPainter::StrokeEllipse(const Rect& outline_rect,
                                           IBrush* brush, float width) {
  Validate();

  QuartzBrush* b = CheckPlatform<QuartzBrush>(brush, GetPlatformId());
  b->Select(cg_context_);
  SetLineWidth(width);

  CGContextStrokeEllipseInRect(cg_context_, Convert(outline_rect));
}

void QuartzCGContextPainter::FillEllipse(const Rect& outline_rect,
                                         IBrush* brush, float width) {
  Validate();

  QuartzBrush* b = CheckPlatform<QuartzBrush>(brush, GetPlatformId());
  b->Select(cg_context_);
  CGContextFillEllipseInRect(cg_context_, Convert(outline_rect));
}

void QuartzCGContextPainter::StrokeGeometry(IGeometry* geometry, IBrush* brush,
                                            float width) {
  Validate();

  QuartzGeometry* g = CheckPlatform<QuartzGeometry>(geometry, GetPlatformId());
  QuartzBrush* b = CheckPlatform<QuartzBrush>(brush, GetPlatformId());

  b->Select(cg_context_);
  SetLineWidth(width);

  CGContextBeginPath(cg_context_);
  CGContextAddPath(cg_context_, g->GetCGPath());
  CGContextStrokePath(cg_context_);
}

void QuartzCGContextPainter::FillGeometry(IGeometry* geometry, IBrush* brush) {
  Validate();

  QuartzGeometry* g = CheckPlatform<QuartzGeometry>(geometry, GetPlatformId());
  QuartzBrush* b = CheckPlatform<QuartzBrush>(brush, GetPlatformId());

  b->Select(cg_context_);
  CGContextBeginPath(cg_context_);
  CGContextAddPath(cg_context_, g->GetCGPath());
  CGContextEOFillPath(cg_context_);
}

void QuartzCGContextPainter::DrawText(const Point& offset,
                                      ITextLayout* text_layout, IBrush* brush) {
  Validate();

  auto tl = CheckPlatform<OsxCTTextLayout>(text_layout, GetPlatformId());

  Color color;

  if (auto b = dynamic_cast<QuartzSolidColorBrush*>(brush)) {
    color = b->GetColor();
  } else {
    color = colors::black;
  }

  auto bounds = tl->GetTextBounds();

  bounds.width += bounds.left;
  bounds.height += bounds.top;
  bounds.left = bounds.top = 0;

  Matrix transform =
      Matrix::Translation(-bounds.width / 2, -bounds.height / 2) *
      Matrix::Scale(1, -1) *
      Matrix::Translation(bounds.width / 2, bounds.height / 2);

  CGContextSaveGState(cg_context_);

  CGContextConcatCTM(cg_context_, Convert(transform * Matrix::Translation(
                                                          offset.x, offset.y)));

  auto frame = tl->CreateFrameWithColor(color);
  Ensures(frame);
  CTFrameDraw(frame, cg_context_);
  CFRelease(frame);

  CGContextRestoreGState(cg_context_);
}

void QuartzCGContextPainter::PushLayer(const Rect& bounds) {
  Validate();
  clip_stack_.push_back(bounds);
  CGContextClipToRect(cg_context_, Convert(bounds));
}

void QuartzCGContextPainter::PopLayer() {
  Validate();
  clip_stack_.pop_back();
  if (clip_stack_.empty()) {
    CGContextResetClip(cg_context_);
  } else {
    CGContextClipToRect(cg_context_, Convert(clip_stack_.back()));
  }
}

void QuartzCGContextPainter::PushState() {
  Validate();
  CGContextSaveGState(cg_context_);
}

void QuartzCGContextPainter::PopState() {
  Validate();
  CGContextRestoreGState(cg_context_);
}

void QuartzCGContextPainter::EndDraw() { DoEndDraw(); }

void QuartzCGContextPainter::SetLineWidth(float width) {
  if (cg_context_) {
    CGContextSetLineWidth(cg_context_, width);
  }
}

void QuartzCGContextPainter::DoEndDraw() {
  if (cg_context_) {
    CGContextFlush(cg_context_);
    CGContextSynchronize(cg_context_);
    // log::TagDebug(log_tag, u"End draw and flush.");

    on_end_draw_(this);
  }
}

void QuartzCGContextPainter::Validate() {
  if (cg_context_ == nullptr)
    throw ReuseException(u"QuartzCGContextPainter has already be released.");
}
}  // namespace cru::platform::graphics::osx::quartz
