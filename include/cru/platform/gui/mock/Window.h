#pragma once

#include "Base.h"
#include "Cursor.h"
#include "InputMethod.h"

#include <cru/base/io/Stream.h>
#include <cru/platform/graphics/Image.h>
#include <cru/platform/graphics/ImageFactory.h>
#include <cru/platform/gui/Window.h>

#include <memory>
#include <string>
#include <string_view>

namespace cru::platform::gui::mock {
class MockUiApplication;

/**
 * @brief Mock native window owned by MockUiApplication.
 *
 * Tests usually create it through MockUiApplication::CreateMockWindow, set
 * client size and visibility, then pump or settle the app after event injection
 * or repaint requests.
 *
 * IME helpers update mock state and raise native events. Use MockUiApplication
 * or MockUser for resize, visibility, mouse, focus, key, and text actions; use
 * these low-level helpers when tests need exact IME control.
 *
 * RequestRepaint is asynchronous and coalesced. BeginPaint paints into a bitmap
 * created from the app's injected graphics factory. GetSnapshotImage,
 * CloneSnapshotToBitmap, and EncodeSnapshotToStream require a successful paint;
 * missing stream/factory/image support and before-paint access throw
 * diagnostics that include compact window state, snapshot dimensions, and paint
 * count. Snapshot bytes and encoder availability depend on the injected
 * graphics backend, so snapshots are for direct inspection or ad hoc capture
 * rather than a built-in golden baseline, image diff, or update workflow.
 */
class CRU_PLATFORM_GUI_MOCK_API MockWindow : public MockResource,
                                             public virtual INativeWindow {
 public:
  static constexpr Thickness kDefaultBorderSize{8.f, 30.f, 8.f, 8.f};

  explicit MockWindow(MockUiApplication* application);
  ~MockWindow() override;

  bool IsCreated() override;
  void Close() override;

  INativeWindow* GetParent() override;
  void SetParent(INativeWindow* parent) override;

  WindowStyleFlag GetStyleFlag() override;
  void SetStyleFlag(WindowStyleFlag flag) override;

  std::string GetTitle() override;
  void SetTitle(std::string title) override;

  WindowVisibilityType GetVisibility() override;
  void SetVisibility(WindowVisibilityType visibility) override;

  Size GetClientSize() override;
  void SetClientSize(const Size& size) override;

  Rect GetClientRect() override;
  void SetClientRect(const Rect& rect) override;

  Rect GetWindowRect() override;
  void SetWindowRect(const Rect& rect) override;

  bool RequestFocus() override;

  Point GetMousePosition() override;

  Thickness GetBorderSize() const { return border_size_; }
  void SetBorderSize(const Thickness& border_size);
  Thickness GetEffectiveBorderSize() const;
  Point ClientToScreen(const Point& point) const;
  Point ScreenToClient(const Point& point) const;
  bool IsScreenPointInWindow(const Point& point);
  bool IsScreenPointInClient(const Point& point);

  bool CaptureMouse() override;
  bool ReleaseMouse() override;

  void SetCursor(std::shared_ptr<ICursor> cursor) override;

  void SetToForeground() override;

  void RequestRepaint() override;

  std::unique_ptr<graphics::IPainter> BeginPaint() override;

  CRU_DEFINE_CRU_PLATFORM_GUI_I_NATIVE_WINDOW_OVERRIDE_EVENTS()

  IInputMethodContext* GetInputMethodContext() override;

  MockUiApplication* GetMockUiApplication() { return application_; }
  MockInputMethodContext* GetMockInputMethodContext() {
    return &input_method_context_;
  }
  bool HasFocus() const;
  bool HasMouseCapture() const;
  bool IsMouseInside() const;
  std::shared_ptr<ICursor> GetCursor() const { return cursor_; }
  bool HasPendingRepaint() const { return repaint_pending_; }
  graphics::IImage* GetBackingImage() const { return backing_image_.get(); }
  std::size_t GetPaintCount() const { return paint_count_; }
  const std::string& GetLastInjectedEvent() const {
    return last_injected_event_;
  }
  std::string GetDiagnostic() const;

  graphics::IImage* GetSnapshotImage();
  std::unique_ptr<graphics::IImage> CloneSnapshotToBitmap();
  void EncodeSnapshotToStream(
      io::Stream* stream,
      graphics::ImageFormat format = graphics::ImageFormat::Png,
      float quality = 1.f);
  void InjectCompositionStart();
  void InjectCompositionUpdate(CompositionText composition_text);
  void InjectCompositionUpdate(std::string text,
                               CompositionClauses clauses = {},
                               TextRange selection = {});
  void InjectCompositionEnd();
  void InjectCompositionCommit(std::string text);
  void CompleteComposition();
  void CancelComposition();
  void SetCandidateWindowPosition(const Point& point);

 private:
  friend class MockUiApplication;

  void CreateNativeWindow();
  void RaiseResizeIfCreatedAndSizeChanged(const Size& old_size);
  bool RaiseResize(const Size& client_size);
  bool RaiseVisibilityChange(WindowVisibilityType visibility);
  bool RaiseMouseEnter();
  bool RaiseMouseLeave();
  bool RaiseMouseMove(const Point& point);
  bool RaiseMouseDown(const NativeMouseButtonEventArgs& args);
  bool RaiseMouseDown(MouseButton button, const Point& point,
                      KeyModifier modifier = KeyModifiers::None);
  bool RaiseMouseUp(const NativeMouseButtonEventArgs& args);
  bool RaiseMouseUp(MouseButton button, const Point& point,
                    KeyModifier modifier = KeyModifiers::None);
  bool RaiseMouseWheel(const NativeMouseWheelEventArgs& args);
  bool RaiseMouseWheel(float delta, const Point& point, KeyModifier modifier,
                       bool horizontal);
  bool RaiseFocus(FocusChangeType focus);
  bool RaiseKeyDown(KeyCode key, KeyModifier modifier = KeyModifiers::None);
  bool RaiseKeyUp(KeyCode key, KeyModifier modifier = KeyModifiers::None);
  bool RaiseTextInput(std::string text);
  void RecreateBackingImageIfPossible();
  void EnsureBackingImageForPainting(std::string_view operation);
  void EnsurePaintedForSnapshot(std::string_view operation);
  void RecordInjectedEvent(std::string event);

 private:
  MockUiApplication* application_;
  MockWindow* parent_ = nullptr;
  WindowStyleFlag style_flag_;
  std::string title_;
  WindowVisibilityType visibility_ = WindowVisibilityType::Hide;
  Rect client_rect_{};
  Thickness border_size_ = kDefaultBorderSize;
  std::shared_ptr<MockCursor> cursor_;
  bool created_ = false;
  bool is_closing_ = false;
  bool repaint_pending_ = false;
  long long repaint_action_id_ = 0;
  std::size_t paint_count_ = 0;
  std::string last_injected_event_;
  std::unique_ptr<graphics::IImage> backing_image_;
  MockInputMethodContext input_method_context_;
};
}  // namespace cru::platform::gui::mock
