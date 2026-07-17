#include "cru/platform/gui/mock/Window.h"

#include "cru/platform/gui/mock/UiApplication.h"

#include <cru/base/Base.h>
#include <cru/platform/graphics/Factory.h>
#include <cru/platform/graphics/ImageFactory.h>

#include <algorithm>
#include <cmath>
#include <format>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace cru::platform::gui::mock {
namespace {
int BitmapDimension(float value) {
  return std::max(0, static_cast<int>(std::ceil(value)));
}

cru::Exception MissingGraphicsFactoryException(const MockWindow& window,
                                               std::string_view operation) {
  return cru::Exception(std::format(
      "MockWindow {} requires a graphics factory on MockUiApplication. "
      "window={{{}}}",
      operation, window.GetDiagnostic()));
}

cru::Exception MissingImageFactoryException(const MockWindow& window,
                                            std::string_view operation) {
  return cru::Exception(std::format(
      "MockWindow {} requires graphics factory image factory support. "
      "window={{{}}}",
      operation, window.GetDiagnostic()));
}

cru::Exception MissingSnapshotStreamException(const MockWindow& window) {
  return cru::Exception(std::format(
      "MockWindow snapshot encoding requires a non-null stream. window={{{}}}",
      window.GetDiagnostic()));
}

const char* ToString(WindowVisibilityType visibility) {
  switch (visibility) {
    case WindowVisibilityType::Hide:
      return "Hide";
    case WindowVisibilityType::Show:
      return "Show";
    case WindowVisibilityType::Minimize:
      return "Minimize";
    default:
      return "Unknown";
  }
}
}  // namespace

MockWindow::MockWindow(MockUiApplication* application)
    : application_(application) {}

MockWindow::~MockWindow() { Close(); }

bool MockWindow::IsCreated() { return created_; }

void MockWindow::Close() {
  if (!created_ || is_closing_) return;

  is_closing_ = true;
  if (repaint_action_id_ > 0) {
    application_->CancelTimer(repaint_action_id_);
  }
  repaint_pending_ = false;
  repaint_action_id_ = 0;
  DestroyEvent_.Raise(nullptr);

  created_ = false;
  closed_ = true;
  is_closing_ = false;
  visibility_ = WindowVisibilityType::Hide;
  has_focus_ = false;
  has_mouse_capture_ = false;
  is_mouse_inside_ = false;
  backing_image_.reset();
  paint_count_ = 0;
  repaint_pending_ = false;
  repaint_action_id_ = 0;
  application_->UnregisterWindow(this);

  if (application_->IsQuitOnAllWindowClosed() &&
      application_->GetAllWindow().empty()) {
    application_->RequestQuit(0);
  }
}

INativeWindow* MockWindow::GetParent() { return parent_; }

void MockWindow::SetParent(INativeWindow* parent) {
  parent_ = CheckPlatform<MockWindow>(parent, GetPlatformId());
}

WindowStyleFlag MockWindow::GetStyleFlag() { return style_flag_; }

void MockWindow::SetStyleFlag(WindowStyleFlag flag) { style_flag_ = flag; }

std::string MockWindow::GetTitle() { return title_; }

void MockWindow::SetTitle(std::string title) { title_ = std::move(title); }

WindowVisibilityType MockWindow::GetVisibility() { return visibility_; }

void MockWindow::SetVisibility(WindowVisibilityType visibility) {
  if (visibility == WindowVisibilityType::Hide) {
    if (visibility_ == WindowVisibilityType::Hide) return;
    visibility_ = WindowVisibilityType::Hide;
    VisibilityChangeEvent_.Raise(WindowVisibilityType::Hide);
    return;
  }

  if (visibility == WindowVisibilityType::Minimize) {
    if (visibility_ == WindowVisibilityType::Minimize) return;
    visibility_ = WindowVisibilityType::Minimize;
    VisibilityChangeEvent_.Raise(WindowVisibilityType::Minimize);
    return;
  }

  if (!created_) {
    CreateNativeWindow();
  }

  if (visibility_ == visibility) return;
  visibility_ = visibility;
  VisibilityChangeEvent_.Raise(visibility);
}

Size MockWindow::GetClientSize() { return client_rect_.GetSize(); }

void MockWindow::SetClientSize(const Size& size) {
  const auto old_size = GetClientSize();
  client_rect_.SetSize(size);
  window_rect_.SetSize(size);
  RaiseResizeIfCreatedAndSizeChanged(old_size);
}

Rect MockWindow::GetClientRect() { return client_rect_; }

void MockWindow::SetClientRect(const Rect& rect) {
  const auto old_size = GetClientSize();
  client_rect_ = rect;
  window_rect_ = rect;
  RaiseResizeIfCreatedAndSizeChanged(old_size);
}

Rect MockWindow::GetWindowRect() { return window_rect_; }

void MockWindow::SetWindowRect(const Rect& rect) {
  const auto old_size = GetClientSize();
  window_rect_ = rect;
  client_rect_ = rect;
  RaiseResizeIfCreatedAndSizeChanged(old_size);
}

bool MockWindow::RequestFocus() {
  if (!created_) return false;
  if (!has_focus_) {
    has_focus_ = true;
    FocusEvent_.Raise(FocusChangeType::Gain);
  }
  return true;
}

Point MockWindow::GetMousePosition() { return mouse_position_; }

void MockWindow::SetMousePosition(const Point& point) {
  mouse_position_ = point;
}

bool MockWindow::CaptureMouse() {
  if (!created_) return false;
  has_mouse_capture_ = true;
  return true;
}

bool MockWindow::ReleaseMouse() {
  if (!created_) return false;
  has_mouse_capture_ = false;
  return true;
}

void MockWindow::SetCursor(std::shared_ptr<ICursor> cursor) {
  cursor_ = CheckPlatform<MockCursor>(cursor, GetPlatformId());
}

void MockWindow::SetToForeground() {
  if (!created_ || visibility_ != WindowVisibilityType::Show) {
    SetVisibility(WindowVisibilityType::Show);
  }
}

void MockWindow::RequestRepaint() {
  if (!created_ || is_closing_ || repaint_pending_) return;

  repaint_pending_ = true;
  repaint_action_id_ = application_->SetImmediate([this] {
    repaint_pending_ = false;
    repaint_action_id_ = 0;
    if (!created_) return;

    PaintEvent_.Raise(nullptr);
    if (!created_) return;
    NativePaintEventArgs args{{{}, GetClientSize()}};
    Paint1Event_.Raise(args);
  });
}

std::unique_ptr<graphics::IPainter> MockWindow::BeginPaint() {
  EnsureBackingImageForPainting("BeginPaint");
  ++paint_count_;
  return backing_image_->CreatePainter();
}

graphics::IImage* MockWindow::GetSnapshotImage() {
  EnsureBackingImageForPainting("GetSnapshotImage");
  EnsurePaintedForSnapshot("GetSnapshotImage");
  return backing_image_.get();
}

std::unique_ptr<graphics::IImage> MockWindow::CloneSnapshotToBitmap() {
  return GetSnapshotImage()->CloneToBitmap();
}

void MockWindow::EncodeSnapshotToStream(io::Stream* stream,
                                        graphics::ImageFormat format,
                                        float quality) {
  if (stream == nullptr) throw MissingSnapshotStreamException(*this);

  auto* snapshot = GetSnapshotImage();
  auto* graphics_factory = application_->GetGraphicsFactory();
  if (graphics_factory == nullptr) {
    throw MissingGraphicsFactoryException(*this, "EncodeSnapshotToStream");
  }

  auto* image_factory = graphics_factory->GetImageFactory();
  if (image_factory == nullptr) {
    throw MissingImageFactoryException(*this, "EncodeSnapshotToStream");
  }

  image_factory->EncodeToStream(snapshot, stream, format, quality);
}

IInputMethodContext* MockWindow::GetInputMethodContext() {
  return &input_method_context_;
}

bool MockWindow::InjectResize(const Size& client_size) {
  RecordInjectedEvent("InjectResize");
  if (!created_) return false;

  const auto old_size = GetClientSize();
  if (old_size == client_size) return false;

  client_rect_.SetSize(client_size);
  window_rect_.SetSize(client_size);
  RecreateBackingImageIfPossible();
  ResizeEvent_.Raise(client_size);
  return true;
}

bool MockWindow::InjectVisibilityChange(WindowVisibilityType visibility) {
  RecordInjectedEvent("InjectVisibilityChange");
  if (!created_ || visibility_ == visibility) return false;

  visibility_ = visibility;
  VisibilityChangeEvent_.Raise(visibility);
  return true;
}

bool MockWindow::InjectFocus(FocusChangeType focus) {
  RecordInjectedEvent("InjectFocus");
  if (!created_) return false;

  const auto has_focus = focus == FocusChangeType::Gain;
  if (has_focus_ == has_focus) return false;

  has_focus_ = has_focus;
  FocusEvent_.Raise(focus);
  return true;
}

bool MockWindow::InjectMouseEnter() {
  RecordInjectedEvent("InjectMouseEnter");
  if (!created_) return false;

  is_mouse_inside_ = true;
  MouseEnterLeaveEvent_.Raise(MouseEnterLeaveType::Enter);
  return true;
}

bool MockWindow::InjectMouseLeave() {
  RecordInjectedEvent("InjectMouseLeave");
  if (!created_) return false;

  is_mouse_inside_ = false;
  MouseEnterLeaveEvent_.Raise(MouseEnterLeaveType::Leave);
  return true;
}

bool MockWindow::InjectMouseMove(const Point& point) {
  RecordInjectedEvent("InjectMouseMove");
  if (!created_) return false;

  mouse_position_ = point;
  MouseMoveEvent_.Raise(mouse_position_);
  return true;
}

bool MockWindow::InjectMouseDown(const NativeMouseButtonEventArgs& args) {
  RecordInjectedEvent("InjectMouseDown");
  if (!created_) return false;

  mouse_position_ = args.point;
  MouseDownEvent_.Raise(args);
  return true;
}

bool MockWindow::InjectMouseDown(MouseButton button, const Point& point,
                                 KeyModifier modifier) {
  return InjectMouseDown(NativeMouseButtonEventArgs{button, point, modifier});
}

bool MockWindow::InjectMouseUp(const NativeMouseButtonEventArgs& args) {
  RecordInjectedEvent("InjectMouseUp");
  if (!created_) return false;

  mouse_position_ = args.point;
  MouseUpEvent_.Raise(args);
  return true;
}

bool MockWindow::InjectMouseUp(MouseButton button, const Point& point,
                               KeyModifier modifier) {
  return InjectMouseUp(NativeMouseButtonEventArgs{button, point, modifier});
}

bool MockWindow::InjectMouseWheel(const NativeMouseWheelEventArgs& args) {
  RecordInjectedEvent("InjectMouseWheel");
  if (!created_) return false;

  mouse_position_ = args.point;
  MouseWheelEvent_.Raise(args);
  return true;
}

bool MockWindow::InjectMouseWheel(float delta, const Point& point,
                                  KeyModifier modifier, bool horizontal) {
  return InjectMouseWheel(
      NativeMouseWheelEventArgs{delta, point, modifier, horizontal});
}

void MockWindow::InjectKeyDown(KeyCode key, KeyModifier modifier) {
  RecordInjectedEvent("InjectKeyDown");
  NativeKeyEventArgs args{key, modifier};
  KeyDownEvent_.Raise(args);
}

void MockWindow::InjectKeyUp(KeyCode key, KeyModifier modifier) {
  RecordInjectedEvent("InjectKeyUp");
  NativeKeyEventArgs args{key, modifier};
  KeyUpEvent_.Raise(args);
}

void MockWindow::InjectTextInput(std::string text) {
  RecordInjectedEvent("InjectTextInput");
  input_method_context_.EmitText(std::move(text));
}

void MockWindow::InjectCompositionStart() {
  RecordInjectedEvent("InjectCompositionStart");
  input_method_context_.BeginComposition();
}

void MockWindow::InjectCompositionUpdate(CompositionText composition_text) {
  RecordInjectedEvent("InjectCompositionUpdate");
  input_method_context_.UpdateComposition(std::move(composition_text));
}

void MockWindow::InjectCompositionUpdate(std::string text,
                                         CompositionClauses clauses,
                                         TextRange selection) {
  input_method_context_.UpdateComposition(std::move(text), std::move(clauses),
                                          selection);
}

void MockWindow::InjectCompositionEnd() {
  RecordInjectedEvent("InjectCompositionEnd");
  input_method_context_.EndComposition();
}

void MockWindow::InjectCompositionCommit(std::string text) {
  RecordInjectedEvent("InjectCompositionCommit");
  input_method_context_.CommitText(std::move(text));
}

void MockWindow::CompleteComposition() {
  RecordInjectedEvent("CompleteComposition");
  input_method_context_.CompleteComposition();
}

void MockWindow::CancelComposition() {
  RecordInjectedEvent("CancelComposition");
  input_method_context_.CancelComposition();
}

void MockWindow::SetCandidateWindowPosition(const Point& point) {
  RecordInjectedEvent("SetCandidateWindowPosition");
  input_method_context_.SetCandidateWindowPosition(point);
}

std::string MockWindow::GetDiagnostic() const {
  std::ostringstream stream;
  stream << "created=" << (created_ ? "true" : "false")
         << ", closed=" << (closed_ ? "true" : "false")
         << ", visibility=" << ToString(visibility_)
         << ", has_focus=" << (has_focus_ ? "true" : "false")
         << ", mouse_inside=" << (is_mouse_inside_ ? "true" : "false")
         << ", mouse_capture=" << (has_mouse_capture_ ? "true" : "false")
         << ", pending_repaint=" << (repaint_pending_ ? "true" : "false")
         << ", repaint_action_id=" << repaint_action_id_
         << ", paint_count=" << paint_count_ << ", client_rect=("
         << client_rect_.left << ", " << client_rect_.top << ", "
         << client_rect_.width << ", " << client_rect_.height << ")"
         << ", mouse_position=(" << mouse_position_.x << ", "
         << mouse_position_.y << ")";

  if (backing_image_ != nullptr) {
    stream << ", snapshot=" << backing_image_->GetWidth() << "x"
           << backing_image_->GetHeight();
  } else {
    stream << ", snapshot=(none)";
  }

  stream << ", last_injected_event="
         << (last_injected_event_.empty() ? "(none)" : last_injected_event_)
         << ", ime={" << input_method_context_.GetDiagnostic() << "}";
  return stream.str();
}

void MockWindow::CreateNativeWindow() {
  created_ = true;
  closed_ = false;
  is_closing_ = false;
  visibility_ = WindowVisibilityType::Hide;
  application_->RegisterWindow(this);
  RecreateBackingImageIfPossible();
  CreateEvent_.Raise(nullptr);
}

void MockWindow::RaiseResizeIfCreatedAndSizeChanged(const Size& old_size) {
  const auto new_size = GetClientSize();
  if (created_ && old_size != new_size) {
    RecreateBackingImageIfPossible();
    ResizeEvent_.Raise(new_size);
  }
}

void MockWindow::RecreateBackingImageIfPossible() {
  auto* graphics_factory = application_->GetGraphicsFactory();
  if (graphics_factory == nullptr) {
    backing_image_.reset();
    paint_count_ = 0;
    return;
  }

  auto* image_factory = graphics_factory->GetImageFactory();
  if (image_factory == nullptr) {
    backing_image_.reset();
    paint_count_ = 0;
    return;
  }

  const auto size = GetClientSize();
  backing_image_ = image_factory->CreateBitmap(BitmapDimension(size.width),
                                               BitmapDimension(size.height));
  paint_count_ = 0;
}

void MockWindow::EnsureBackingImageForPainting(std::string_view operation) {
  if (backing_image_ != nullptr) return;

  auto* graphics_factory = application_->GetGraphicsFactory();
  if (graphics_factory == nullptr) {
    throw MissingGraphicsFactoryException(*this, operation);
  }

  auto* image_factory = graphics_factory->GetImageFactory();
  if (image_factory == nullptr) {
    throw MissingImageFactoryException(*this, operation);
  }

  const auto size = GetClientSize();
  backing_image_ = image_factory->CreateBitmap(BitmapDimension(size.width),
                                               BitmapDimension(size.height));
}

void MockWindow::EnsurePaintedForSnapshot(std::string_view operation) {
  if (paint_count_ > 0) return;

  throw Exception(std::format(
      "MockWindow {} requires at least one BeginPaint before snapshot access. "
      "window={{{}}}",
      operation, GetDiagnostic()));
}

void MockWindow::RecordInjectedEvent(std::string event) {
  last_injected_event_ = std::move(event);
}
}  // namespace cru::platform::gui::mock
