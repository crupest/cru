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

  std::optional<TextRange> GetSelection() {
    return this->control_->GetTextRenderObject()->GetSelectionRange();
  }
  void SetSelection(std::optional<TextRange> selection) {
    this->control_->GetTextRenderObject()->SetSelectionRange(selection);
  }

 private:
  void AbortSelection() {
    if (this->select_down_button_.has_value()) {
      this->control_->ReleaseMouse();
      this->select_down_button_ = std::nullopt;
    }
    this->control_->GetTextRenderObject()->SetSelectionRange(std::nullopt);
  }

  void SetupCaret() {
    const auto application = GetUiApplication();

    // Cancel first anyhow for safety.
    application->CancelTimer(this->caret_timer_id_);

    this->control_->GetTextRenderObject()->SetDrawCaret(true);
    this->caret_timer_id_ = application->SetInterval(
        std::chrono::milliseconds(this->caret_blink_duration_),
        [this] { this->control_->GetTextRenderObject()->ToggleDrawCaret(); });
  }

  void TearDownCaret() {
    const auto application = GetUiApplication();
    application->CancelTimer(this->caret_timer_id_);
    this->control_->GetTextRenderObject()->SetDrawCaret(false);
  }

  template <typename TArgs>
  void SetupOneHandler(event::RoutedEvent<TArgs>* (TControl::*event)(),
                       void (TextControlService::*handler)(
                           typename event::RoutedEvent<TArgs>::EventArgs)) {
    this->event_revoker_guards_.push_back(
        EventRevokerGuard{(this->control_->*event)()->Direct()->AddHandler(
            std::bind(handler, this, std::placeholders::_1))});
  }

  void SetupHandlers() {
    Expects(event_revoker_guards_.empty());

    SetupOneHandler(&Control::MouseMoveEvent,
                    &TextControlService::MouseMoveHandler);
    SetupOneHandler(&Control::MouseDownEvent,
                    &TextControlService::MouseDownHandler);
    SetupOneHandler(&Control::MouseUpEvent,
                    &TextControlService::MouseUpHandler);
    SetupOneHandler(&Control::LoseFocusEvent,
                    &TextControlService::LoseFocusHandler);
  }

  void MouseMoveHandler(event::MouseEventArgs& args) {
    if (this->select_down_button_.has_value()) {
      const auto text_render_object = this->control_->GetTextRenderObject();
      const auto result = text_render_object->TextHitTest(
          text_render_object->FromRootToContent(args.GetPoint()));
      const auto position = result.position + (result.trailing ? 1 : 0);
      log::TagDebug(log_tag,
                    "Text selection changed on mouse move, range: {}, {}.",
                    position, this->select_start_position_);
      this->control_->GetTextRenderObject()->SetSelectionRange(
          TextRange::FromTwoSides(position, this->select_start_position_));
      text_render_object->SetCaretPosition(position);
    }
  }

  void MouseDownHandler(event::MouseButtonEventArgs& args) {
    if (this->select_down_button_.has_value()) {
      return;
    } else {
      if (!this->control_->CaptureMouse()) return;
      if (!this->control_->RequestFocus()) return;
      const auto text_render_object = this->control_->GetTextRenderObject();
      this->select_down_button_ = args.GetButton();
      const auto result = text_render_object->TextHitTest(
          text_render_object->FromRootToContent(args.GetPoint()));
      const auto position = result.position + (result.trailing ? 1 : 0);
      text_render_object->SetSelectionRange(std::nullopt);
      text_render_object->SetCaretPosition(position);
      this->select_start_position_ = position;
      log::TagDebug(log_tag, "Begin to select text, start position: {}.",
                    position);
    }
  }

  void MouseUpHandler(event::MouseButtonEventArgs& args) {
    if (this->select_down_button_.has_value() &&
        this->select_down_button_.value() == args.GetButton()) {
      this->control_->ReleaseMouse();
      this->select_down_button_ = std::nullopt;
      log::TagDebug(log_tag, "End selecting text.");
    }
  }

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

  // before the char
  int select_start_position_;
};
}  // namespace cru::ui::controls
