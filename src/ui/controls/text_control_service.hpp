#pragma once
#include "../helper.hpp"
#include "cru/common/logger.hpp"
#include "cru/platform/graph/font.hpp"
#include "cru/platform/graph/painter.hpp"
#include "cru/platform/native/ui_application.hpp"
#include "cru/ui/control.hpp"
#include "cru/ui/render/canvas_render_object.hpp"
#include "cru/ui/render/text_render_object.hpp"
#include "cru/ui/ui_event.hpp"

namespace cru::ui::controls {
constexpr float caret_width = 2;
constexpr long long caret_blink_duration = 500;

// TControl should inherits `Control` and has following methods:
// ```
// render::TextRenderObject* GetTextRenderObject();
// ```
template <typename TControl>
class TextControlService : public Object {
 public:
  TextControlService(TControl* control);

  CRU_DELETE_COPY(TextControlService)
  CRU_DELETE_MOVE(TextControlService)

  ~TextControlService();

 public:
  bool IsEnabled() { return enable_; }
  void SetEnabled(bool enable);

  int GetCaretPosition() { return caret_position_; }
  void SetCaretPosition(int position) { caret_position_ = position; }

  bool IsCaretVisible() { return caret_visible_; }
  void SetCaretVisible(bool visible);

  // please set correct offset before calling this
  void DrawCaret(platform::graph::IPainter* painter);

 private:
  void AbortSelection();

  void SetupCaret();
  void TearDownCaret();

  void SetupHandlers();

  void MouseMoveHandler(event::MouseEventArgs& args);
  void MouseDownHandler(event::MouseButtonEventArgs& args);
  void MouseUpHandler(event::MouseButtonEventArgs& args);
  void LoseFocusHandler(event::FocusChangeEventArgs& args);

 private:
  TControl* control_;
  std::vector<EventRevokerGuard> event_revoker_guards_;

  bool enable_ = false;

  bool caret_visible_ = false;
  int caret_position_ = 0;
  long long caret_timer_id_ = -1;

  // nullopt means not selecting
  std::optional<MouseButton> select_down_button_;

  // before the char
  int select_start_position_;
};

template <typename TControl>
TextControlService<TControl>::TextControlService(TControl* control)
    : control_(control) {}

template <typename TControl>
TextControlService<TControl>::~TextControlService() {
  const auto application = GetUiApplication();
  // Don't call TearDownCaret, because it use text render object of control,
  // which may be destroyed already.
  application->CancelTimer(this->caret_timer_id_);
}

template <typename TControl>
void TextControlService<TControl>::SetEnabled(bool enable) {
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

template <typename TControl>
void TextControlService<TControl>::SetCaretVisible(bool visible) {
  if (visible == this->caret_visible_) return;

  this->caret_visible_ = visible;

  if (this->enable_) {
    if (visible) {
      this->SetupCaretTimer();
    } else {
      this->TearDownCaretTimer();
    }
  }
}  // namespace cru::ui::controls

template <typename TControl>
void TextControlService<TControl>::AbortSelection() {
  if (this->select_down_button_.has_value()) {
    this->control_->ReleaseMouse();
    this->select_down_button_ = std::nullopt;
  }
  this->control_->GetTextRenderObject()->SetSelectionRange(std::nullopt);
}

template <typename TControl>
void TextControlService<TControl>::SetupCaret() {
  const auto application = GetUiApplication();

  // Cancel first anyhow for safety.
  application->CancelTimer(this->caret_timer_id_);

  this->control_->GetTextRenderObject()->SetDrawCaret(true);
  this->caret_timer_id_ = application->SetInterval(
      std::chrono::milliseconds(caret_blink_duration),
      [this] { this->control_->GetTextRenderObject()->ToggleDrawCaret(); });
}

template <typename TControl>
void TextControlService<TControl>::TearDownCaret() {
  const auto application = GetUiApplication();
  application->CancelTimer(this->caret_timer_id_);
  this->control_->GetTextRenderObject()->SetDrawCaret(false);
}

template <typename TControl>
void TextControlService<TControl>::SetupHandlers() {
  Expects(event_revoker_guards_.empty());
  this->event_revoker_guards_.push_back(
      EventRevokerGuard{control_->MouseMoveEvent()->Direct()->AddHandler(
          std::bind(&TextControlService::MouseMoveHandler, this,
                    std::placeholders::_1))});
  this->event_revoker_guards_.push_back(
      EventRevokerGuard{control_->MouseDownEvent()->Direct()->AddHandler(
          std::bind(&TextControlService::MouseDownHandler, this,
                    std::placeholders::_1))});
  this->event_revoker_guards_.push_back(EventRevokerGuard{
      control_->MouseUpEvent()->Direct()->AddHandler(std::bind(
          &TextControlService::MouseUpHandler, this, std::placeholders::_1))});
  this->event_revoker_guards_.push_back(
      EventRevokerGuard{control_->LoseFocusEvent()->Direct()->AddHandler(
          std::bind(&TextControlService::LoseFocusHandler, this,
                    std::placeholders::_1))});
}

template <typename TControl>
void TextControlService<TControl>::MouseMoveHandler(
    event::MouseEventArgs& args) {
  if (this->select_down_button_.has_value()) {
    const auto text_render_object = this->control_->GetTextRenderObject();
    const auto result = text_render_object->TextHitTest(
        text_render_object->FromRootToContent(args.GetPoint()));
    const auto position = result.position + (result.trailing ? 1 : 0);
    this->caret_position_ = position;
    log::Debug(
        "TextControlService: Text selection changed on mouse move, range: {}, "
        "{}.",
        position, this->select_start_position_);
    this->control_->GetTextRenderObject()->SetSelectionRange(
        TextRange::FromTwoSides(
            static_cast<unsigned>(position),
            static_cast<unsigned>(this->select_start_position_)));
  }
}

template <typename TControl>
void TextControlService<TControl>::MouseDownHandler(
    event::MouseButtonEventArgs& args) {
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
    this->select_start_position_ = position;
    log::Debug("TextControlService: Begin to select text, start position: {}.",
               position);
  }
}

template <typename TControl>
void TextControlService<TControl>::MouseUpHandler(
    event::MouseButtonEventArgs& args) {
  if (this->select_down_button_.has_value() &&
      this->select_down_button_.value() == args.GetButton()) {
    this->control_->ReleaseMouse();
    this->select_down_button_ = std::nullopt;
    log::Debug("TextControlService: End selecting text.");
  }
}

template <typename TControl>
void TextControlService<TControl>::LoseFocusHandler(
    event::FocusChangeEventArgs& args) {
  if (!args.IsWindow()) this->AbortSelection();
}
}  // namespace cru::ui::controls
