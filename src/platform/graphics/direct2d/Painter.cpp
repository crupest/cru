#include "cru/platform/graphics/direct2d/Painter.h"
#include "cru/base/log/Logger.h"
#include "cru/platform/graphics/direct2d/Base.h"
#include "cru/platform/graphics/direct2d/Brush.h"
#include "cru/platform/graphics/direct2d/Geometry.h"
#include "cru/platform/graphics/direct2d/Image.h"
#include "cru/platform/graphics/direct2d/TextLayout.h"

namespace cru::platform::graphics::direct2d {
D2DDeviceContextPainter::D2DDeviceContextPainter(
    DirectGraphicsFactory* graphics_factory,
    ID2D1DeviceContext1* device_context, bool release)
    : DirectGraphicsResource(graphics_factory) {
  Expects(device_context);
  device_context_ = device_context;
  release_ = release;
  device_context->BeginDraw();
}

D2DDeviceContextPainter::~D2DDeviceContextPainter() {
  if (is_drawing_) {
    CRU_LOG_TAG_INFO(
        "You may forget to call EndDraw before destroying painter.");
  }

  if (release_) {
    device_context_->Release();
    device_context_ = nullptr;
  }
}

platform::Matrix D2DDeviceContextPainter::GetTransform() {
  CheckValidation();
  D2D1_MATRIX_3X2_F m;
  device_context_->GetTransform(&m);
  return Convert(m);
}

void D2DDeviceContextPainter::SetTransform(const platform::Matrix& matrix) {
  CheckValidation();
  device_context_->SetTransform(Convert(matrix));
}

void D2DDeviceContextPainter::ConcatTransform(const Matrix& matrix) {
  SetTransform(GetTransform() * matrix);
}

void D2DDeviceContextPainter::Clear(const Color& color) {
  CheckValidation();
  device_context_->Clear(Convert(color));
}

void D2DDeviceContextPainter::DrawLine(const Point& start, const Point& end,
                                       IBrush* brush, float width) {
  CheckValidation();
  const auto b = CheckPlatform<ID2DBrush>(brush, GetPlatformId());
  device_context_->DrawLine(Convert(start), Convert(end),
                            b->GetD2DBrushInterface(), width);
}

void D2DDeviceContextPainter::StrokeRectangle(const Rect& rectangle,
                                              IBrush* brush, float width) {
  CheckValidation();
  const auto b = CheckPlatform<ID2DBrush>(brush, GetPlatformId());
  device_context_->DrawRectangle(Convert(rectangle), b->GetD2DBrushInterface(),
                                 width);
}

void D2DDeviceContextPainter::FillRectangle(const Rect& rectangle,
                                            IBrush* brush) {
  CheckValidation();
  const auto b = CheckPlatform<ID2DBrush>(brush, GetPlatformId());
  device_context_->FillRectangle(Convert(rectangle), b->GetD2DBrushInterface());
}

void D2DDeviceContextPainter::StrokeEllipse(const Rect& outline_rect,
                                            IBrush* brush, float width) {
  CheckValidation();
  const auto b = CheckPlatform<ID2DBrush>(brush, GetPlatformId());
  device_context_->DrawEllipse(
      D2D1::Ellipse(Convert(outline_rect.GetCenter()),
                    outline_rect.width / 2.0f, outline_rect.height / 2.0f),
      b->GetD2DBrushInterface(), width);
}
void D2DDeviceContextPainter::FillEllipse(const Rect& outline_rect,
                                          IBrush* brush) {
  CheckValidation();
  const auto b = CheckPlatform<ID2DBrush>(brush, GetPlatformId());
  device_context_->FillEllipse(
      D2D1::Ellipse(Convert(outline_rect.GetCenter()),
                    outline_rect.width / 2.0f, outline_rect.height / 2.0f),
      b->GetD2DBrushInterface());
}

void D2DDeviceContextPainter::StrokeGeometry(IGeometry* geometry, IBrush* brush,
                                             float width) {
  CheckValidation();
  const auto g = CheckPlatform<D2DGeometry>(geometry, GetPlatformId());
  const auto b = CheckPlatform<ID2DBrush>(brush, GetPlatformId());
  device_context_->DrawGeometry(g->GetComInterface(), b->GetD2DBrushInterface(),
                                width);
}

void D2DDeviceContextPainter::FillGeometry(IGeometry* geometry, IBrush* brush) {
  CheckValidation();
  const auto g = CheckPlatform<D2DGeometry>(geometry, GetPlatformId());
  const auto b = CheckPlatform<ID2DBrush>(brush, GetPlatformId());
  device_context_->FillGeometry(g->GetComInterface(),
                                b->GetD2DBrushInterface());
}

void D2DDeviceContextPainter::DrawText(const Point& offset,
                                       ITextLayout* text_layout,
                                       IBrush* brush) {
  CheckValidation();
  const auto t = CheckPlatform<DWriteTextLayout>(text_layout, GetPlatformId());
  const auto b = CheckPlatform<ID2DBrush>(brush, GetPlatformId());
  device_context_->DrawTextLayout(Convert(offset), t->GetComInterface(),
                                  b->GetD2DBrushInterface());
}

void D2DDeviceContextPainter::DrawImage(const Point& offset, IImage* image) {
  CheckValidation();
  const auto i = CheckPlatform<Direct2DImage>(image, GetPlatformId());

  Microsoft::WRL::ComPtr<ID2D1DeviceContext> device_context;

  device_context_->QueryInterface(device_context.GetAddressOf());
  device_context->DrawImage(i->GetD2DBitmap().Get(), Convert(offset));
}

void D2DDeviceContextPainter::PushLayer(const Rect& bounds) {
  CheckValidation();

  Microsoft::WRL::ComPtr<ID2D1Layer> layer;
  ThrowIfFailed(device_context_->CreateLayer(&layer));

  device_context_->PushLayer(D2D1::LayerParameters(Convert(bounds)),
                             layer.Get());

  layers_.push_back(std::move(layer));
}

void D2DDeviceContextPainter::PopLayer() {
  device_context_->PopLayer();
  layers_.pop_back();
}

void D2DDeviceContextPainter::PushState() {
  Microsoft::WRL::ComPtr<ID2D1Factory> factory = nullptr;
  device_context_->GetFactory(&factory);

  Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock> state_block;
  factory->CreateDrawingStateBlock(&state_block);
  device_context_->SaveDrawingState(state_block.Get());

  drawing_state_stack_.push_back(std::move(state_block));
}

void D2DDeviceContextPainter::PopState() {
  Expects(!drawing_state_stack_.empty());
  auto drawing_state = drawing_state_stack_.back();
  drawing_state_stack_.pop_back();
  device_context_->RestoreDrawingState(drawing_state.Get());
}

void D2DDeviceContextPainter::EndDraw() {
  if (is_drawing_) {
    is_drawing_ = false;
    ThrowIfFailed(device_context_->EndDraw());
    DoEndDraw();
  }
}

void D2DDeviceContextPainter::CheckValidation() {
  if (!is_drawing_) {
    throw cru::platform::ReuseException(
        "Can't do that on painter after end drawing.");
  }
}
}  // namespace cru::platform::graphics::direct2d
