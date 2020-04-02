#pragma once
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
// render::CanvasRenderObject* GetCaretRenderObject();
// std::shared_ptr<platform::graph::IBrush> GetCaretBrush();
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

  void SetupCaretTimer();
  void TearDownCaretTimer();

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
#ifdef CRU_DEBUG
  bool caret_timer_set_ = false;
#endif
  unsigned long caret_timer_tag_;
  // this is used for blinking of caret
  bool caret_show_ = true;

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
  if (enable_ && caret_visible_) TearDownCaretTimer();
}

template <typename TControl>
void TextControlService<TControl>::SetEnabled(bool enable) {
  if (enable == enable_) return;
  if (enable) {
    AbortSelection();
    SetupHandlers();
    if (caret_visible_) {
      SetupCaretTimer();
    }
  } else {
    event_revoker_guards_.clear();
    if (caret_visible_) {
      TearDownCaretTimer();
    }
  }
}

template <typename TControl>
void TextControlService<TControl>::SetCaretVisible(bool visible) {
  if (visible == caret_visible_) return;

  if (enable_) {
    if (visible) {
      SetupCaretTimer();
    }
  } else {
    TearDownCaretTimer();
  }
}  // namespace cru::ui::controls

template <typename TControl>
void TextControlService<TControl>::DrawCaret(
    platform::graph::IPainter* painter) {
  if (caret_show_) {
    const auto text_render_object = control_->GetTextRenderObject();
    const auto point = text_render_object->TextSingleRect(
        caret_position_, false);  // Maybe cache the result???
    painter->FillRectangle(
        Rect{point,
             Size{caret_width, text_render_object->GetFont()->GetFontSize()}},
        control_->GetCaretBrush().get());
  }
}

template <typename TControl>
void TextControlService<TControl>::AbortSelection() {
  if (select_down_button_.has_value()) {
    control_->ReleaseMouse();
    select_down_button_ = std::nullopt;
  }
  control_->GetTextRenderObject()->SetSelectionRange(std::nullopt);
}

template <typename TControl>
void TextControlService<TControl>::SetupCaretTimer() {
#ifdef CRU_DEBUG
  Expects(!caret_timer_set_);
  caret_timer_set_ = true;
#endif
  caret_timer_tag_ =
      platform::native::IUiApplication::GetInstance()->SetInterval(
          std::chrono::milliseconds(caret_blink_duration), [this] {
            this->caret_show_ = !this->caret_show_;
            this->control_->GetCaretRenderObject()->InvalidatePaint();
          });
}

template <typename TControl>
void TextControlService<TControl>::TearDownCaretTimer() {
#ifdef CRU_DEBUG
  Expects(!caret_timer_set_);
  caret_timer_set_ = false;
#endif
  platform::native::IUiApplication::GetInstance()->CancelTimer(
      caret_timer_tag_);
}

template <typename TControl>
void TextControlService<TControl>::SetupHandlers() {
  Expects(event_revoker_guards_.empty());
  event_revoker_guards_.push_back(
      EventRevokerGuard{control_->MouseMoveEvent()->Direct()->AddHandler(
          std::bind(&TextControlService::MouseMoveHandler, this,
                    std::placeholders::_1))});
  event_revoker_guards_.push_back(
      EventRevokerGuard{control_->MouseDownEvent()->Direct()->AddHandler(
          std::bind(&TextControlService::MouseDownHandler, this,
                    std::placeholders::_1))});
  event_revoker_guards_.push_back(EventRevokerGuard{
      control_->MouseUpEvent()->Direct()->AddHandler(std::bind(
          &TextControlService::MouseUpHandler, this, std::placeholders::_1))});
  event_revoker_guards_.push_back(
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
    this->control_->GetTextRenderObject()->InvalidatePaint();
    this->control_->GetCaretRenderObject()->InvalidatePaint();
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
