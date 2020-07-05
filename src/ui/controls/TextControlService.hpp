#pragma once
#include "../Helper.hpp"
#include "cru/common/Logger.hpp"
#include "cru/platform/graph/Font.hpp"
#include "cru/platform/graph/Painter.hpp"
#include "cru/platform/native/UiApplication.hpp"
#include "cru/ui/Control.hpp"
#include "cru/ui/UiEvent.hpp"
#include "cru/ui/render/CanvasRenderObject.hpp"
#include "cru/ui/render/TextRenderObject.hpp"

namespace cru::ui::controls {
constexpr int k_default_caret_blink_duration = 500;

// TControl should inherits `Control` and has following methods:
// ```
// gsl::not_null<render::TextRenderObject*> GetTextRenderObject();
// render::ScrollRenderObject* GetScrollRenderObject();
// ```
template <typename TControl>
class TextControlService : public Object {
  CRU_DEFINE_CLASS_LOG_TAG("cru::ui::controls::TextControlService")

 public:
  TextControlService(gsl::not_null<TControl*> control) : control_(control) {}

  CRU_DELETE_COPY(TextControlService)
  CRU_DELETE_MOVE(TextControlService)

  ~TextControlService() override {
    const auto application = GetUiApplication();
    // Don't call TearDownCaret, because it use text render object of control,
    // which may be destroyed already.
    application->CancelTimer(this->caret_timer_id_);
  }

 public:
  bool IsEnabled() { return enable_; }

  void SetEnabled(bool enable) {
    if (enable == this->enable_) return;
    if (enable) {
      this->SetupHandlers();
      if (this->caret_visible_) {
        this->SetupCaret();
      }
    } else {
      this->AbortSelection();
      this->event_revoker_guards_.clear();
      this->TearDownCaret();
    }
  }

  bool IsCaretVisible() { return caret_visible_; }

  void SetCaretVisible(bool visible) {
    if (visible == this->caret_visible_) return;

    this->caret_visible_ = visible;

    if (this->enable_) {
      if (visible) {
        this->SetupCaret();
      } else {
        this->TearDownCaret();
      }
    }
  }

  int GetCaretBlinkDuration() { return caret_blink_duration_; }

  void SetCaretBlinkDuration(int milliseconds) {
    if (this->caret_blink_duration_ == milliseconds) return;

    if (this->enable_ && this->caret_visible_) {
      this->TearDownCaret();
      this->SetupCaret();
    }
  }

  gsl::not_null<render::TextRenderObject*> GetTextRenderObject() {
    return this->control_->GetTextRenderObject();
  }

  render::ScrollRenderObject* GetScrollRenderObject() {
    return this->control_->GetScrollRenderObject();
  }

 private:
  void AbortSelection() {
    if (this->select_down_button_.has_value()) {
      this->control_->ReleaseMouse();
      this->select_down_button_ = std::nullopt;
    }
    this->GetTextRenderObject()->SetSelectionRange(std::nullopt);
  }

  void SetupCaret() {
    const auto application = GetUiApplication();

    // Cancel first anyhow for safety.
    application->CancelTimer(this->caret_timer_id_);

    this->GetTextRenderObject()->SetDrawCaret(true);
    this->caret_timer_id_ = application->SetInterval(
        std::chrono::milliseconds(this->caret_blink_duration_),
        [this] { this->GetTextRenderObject()->ToggleDrawCaret(); });
  }

  void TearDownCaret() {
    const auto application = GetUiApplication();
    application->CancelTimer(this->caret_timer_id_);
    this->GetTextRenderObject()->SetDrawCaret(false);
  }

  template <typename TArgs>
  void SetupOneHandler(event::RoutedEvent<TArgs>* (TControl::*event)(),
                       void (TextControlService::*handler)(
                           typename event::RoutedEvent<TArgs>::EventArgs)) {
    this->event_revoker_guards_.push_back(
        EventRevokerGuard{(this->control_->*event)()->Direct()->AddHandler(
            std::bind(handler, this, std::placeholders::_1))});
  }

  void StartSelection(Index start) {
    const auto text_render_object = this->GetTextRenderObject();
    text_render_object->SetSelectionRange(TextRange{start, 0});
    text_render_object->SetCaretPosition(start);
    log::TagDebug(log_tag, "Text selection started, position: {}.", position);
  }

  void UpdateSelection(Index new_end) {
    if (!old_selection.has_value()) return;

    const auto text_render_object = this->GetTextRenderObject();
    const auto old_selection = text_render_object->GetSelectionRange();
    const auto old_start = old_selection->GetStart();
    this->GetTextRenderObject()->SetSelectionRange(
        TextRange::FromTwoSides(old_start, new_end));
    text_render_object->SetCaretPosition(new_end);
    log::TagDebug(log_tag, "Text selection updated, range: {}, {}.", old_start,
                  new_end);
    if (const auto scroll_render_object = this->GetScrollRenderObject()) {
      //TODO: Implement this.      
    }
  }

  void SetupHandlers() {
    Expects(event_revoker_guards_.empty());

    SetupOneHandler(&Control::MouseMoveEvent,
                    &TextControlService::MouseMoveHandler);
    SetupOneHandler(&Control::MouseDownEvent,
                    &TextControlService::MouseDownHandler);
    SetupOneHandler(&Control::MouseUpEvent,
                    &TextControlService::MouseUpHandler);
    SetupOneHandler(&Control::KeyDownEvent,
                    &TextControlService::KeyDownHandler);
    SetupOneHandler(&Control::KeyUpEvent, &TextControlService::KeyUpHandler);
    SetupOneHandler(&Control::LoseFocusEvent,
                    &TextControlService::LoseFocusHandler);
  }

  void MouseMoveHandler(event::MouseEventArgs& args) {
    if (this->select_down_button_.has_value()) {
      const auto text_render_object = this->GetTextRenderObject();
      const auto result = text_render_object->TextHitTest(
          args.GetPointToContent(text_render_object));
      const auto position = result.position + (result.trailing ? 1 : 0);
      UpdateSelection(position);
    }
  }

  void MouseDownHandler(event::MouseButtonEventArgs& args) {
    if (this->select_down_button_.has_value()) {
      return;
    } else {
      if (!this->control_->CaptureMouse()) return;
      if (!this->control_->RequestFocus()) return;
      const auto text_render_object = this->GetTextRenderObject();
      this->select_down_button_ = args.GetButton();
      const auto result = text_render_object->TextHitTest(
          args.GetPointToContent(text_render_object));
      const auto position = result.position + (result.trailing ? 1 : 0);
      StartSelection(position);
    }
  }

  void MouseUpHandler(event::MouseButtonEventArgs& args) {
    if (this->select_down_button_.has_value() &&
        this->select_down_button_.value() == args.GetButton()) {
      this->control_->ReleaseMouse();
      this->select_down_button_ = std::nullopt;
    }
  }

  void KeyDownHandler(event::KeyEventArgs& args) {}

  void KeyUpHandler(event::KeyEventArgs& args) {}

  void LoseFocusHandler(event::FocusChangeEventArgs& args) {
    if (!args.IsWindow()) this->AbortSelection();
  }

 private:
  gsl::not_null<TControl*> control_;
  std::vector<EventRevokerGuard> event_revoker_guards_;

  bool enable_ = false;

  bool caret_visible_ = false;
  long long caret_timer_id_ = -1;
  int caret_blink_duration_ = k_default_caret_blink_duration;

  // nullopt means not selecting
  std::optional<MouseButton> select_down_button_;
};
}  // namespace cru::ui::controls
