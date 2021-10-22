#include "cru/osx/graphics/quartz/Painter.hpp"

#include "cru/common/Logger.hpp"
#include "cru/osx/graphics/quartz/Brush.hpp"
#include "cru/osx/graphics/quartz/Convert.hpp"
#include "cru/osx/graphics/quartz/Geometry.hpp"
#include "cru/osx/graphics/quartz/TextLayout.hpp"
#include "cru/platform/Check.hpp"
#include "cru/platform/Color.hpp"
#include "cru/platform/Exception.hpp"
#include "cru/platform/graphics/util/Painter.hpp"

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

Matrix QuartzCGContextPainter::GetTransform() {
  return Convert(CGContextGetCTM(cg_context_));
}

void QuartzCGContextPainter::SetTransform(const Matrix& matrix) {
  auto old = CGContextGetCTM(cg_context_);
  old = CGAffineTransformInvert(old);
  CGContextConcatCTM(cg_context_, old);
  CGContextConcatCTM(cg_context_, Convert(matrix));
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
  CGContextSetLineWidth(cg_context_, width);
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

void QuartzCGContextPainter::StrokeGeometry(IGeometry* geometry, IBrush* brush,
                                            float width) {
  Validate();

  QuartzGeometry* g = CheckPlatform<QuartzGeometry>(geometry, GetPlatformId());
  QuartzBrush* b = CheckPlatform<QuartzBrush>(brush, GetPlatformId());

  b->Select(cg_context_);
  CGContextSetLineWidth(cg_context_, width);
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
  CGContextFillPath(cg_context_);
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

  util::WithTransform(this, Matrix::Translation(offset),
                      [this, tl, color](IPainter*) {
                        auto frame = tl->CreateFrameWithColor(color);
                        Ensures(frame);
                        CTFrameDraw(frame, cg_context_);
                        CFRelease(frame);
                      });
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

void QuartzCGContextPainter::EndDraw() { DoEndDraw(); }

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
