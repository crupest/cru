#include "cru/ui/controls/text_common.hpp"

#include "cru/common/logger.hpp"
#include "cru/platform/graph/font.hpp"
#include "cru/platform/graph/painter.hpp"
#include "cru/platform/native/ui_application.hpp"
#include "cru/ui/control.hpp"
#include "cru/ui/render/canvas_render_object.hpp"
#include "cru/ui/render/text_render_object.hpp"

#include <cassert>

namespace cru::ui::controls {
using platform::graph::ITextLayout;

constexpr float caret_width = 2;
constexpr long long caret_blink_duration = 500;

TextControlService::TextControlService(Control* control,
                                       ITextControl* text_control)
    : control_(control), text_control_(text_control) {}

TextControlService::~TextControlService() {
  if (enable_ && caret_visible_) TearDownCaretTimer();
}

void TextControlService::SetEnabled(bool enable) {
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

void TextControlService::SetCaretVisible(bool visible) {
  if (visible == caret_visible_) return;

  if (enable_) {
    if (visible) {
      SetupCaretTimer();
    }
  } else {
    TearDownCaretTimer();
  }
}  // namespace cru::ui::controls

void TextControlService::DrawCaret(platform::graph::IPainter* painter) {
  if (caret_show_) {
    const auto text_render_object = text_control_->GetTextRenderObject();
    const auto point = text_render_object->TextSingleRect(
        caret_position_, false);  // Maybe cache the result???
    painter->FillRectangle(
        Rect{point,
             Size{caret_width, text_render_object->GetFont()->GetFontSize()}},
        text_control_->GetCaretBrush());
  }
}

void TextControlService::AbortSelection() {
  if (select_down_button_.has_value()) {
    control_->ReleaseMouse();
    select_down_button_ = std::nullopt;
  }
  text_control_->GetTextRenderObject()->SetSelectionRange(std::nullopt);
}

void TextControlService::SetupCaretTimer() {
#ifdef CRU_DEBUG
  assert(!caret_timer_set_);
  caret_timer_set_ = true;
#endif
  caret_timer_tag_ =
      platform::native::IUiApplication::GetInstance()->SetInterval(
          std::chrono::milliseconds(caret_blink_duration), [this] {
            this->caret_show_ = !this->caret_show_;
            this->text_control_->GetCaretRenderObject()->InvalidatePaint();
          });
}

void TextControlService::TearDownCaretTimer() {
#ifdef CRU_DEBUG
  assert(!caret_timer_set_);
  caret_timer_set_ = false;
#endif
  platform::native::IUiApplication::GetInstance()->CancelTimer(
      caret_timer_tag_);
}

void TextControlService::SetupHandlers() {
  assert(event_revoker_guards_.empty());
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

void TextControlService::MouseMoveHandler(event::MouseEventArgs& args) {
  if (this->select_down_button_.has_value()) {
    const auto text_render_object = this->text_control_->GetTextRenderObject();
    const auto result = text_render_object->TextHitTest(
        text_render_object->FromRootToContent(args.GetPoint()));
    const auto position = result.position + (result.trailing ? 1 : 0);
    this->caret_position_ = position;
    log::Debug(
        "TextControlService: Text selection changed on mouse move, range: {}, "
        "{}.",
        position, this->select_start_position_);
    this->text_control_->GetTextRenderObject()->SetSelectionRange(
        TextRange::FromTwoSides(
            static_cast<unsigned>(position),
            static_cast<unsigned>(this->select_start_position_)));
    this->text_control_->GetTextRenderObject()->InvalidatePaint();
    this->text_control_->GetCaretRenderObject()->InvalidatePaint();
  }
}

void TextControlService::MouseDownHandler(event::MouseButtonEventArgs& args) {
  if (this->select_down_button_.has_value()) {
    return;
  } else {
    if (!this->control_->CaptureMouse()) return;
    if (!this->control_->RequestFocus()) return;
    const auto text_render_object = this->text_control_->GetTextRenderObject();
    this->select_down_button_ = args.GetButton();
    const auto result = text_render_object->TextHitTest(
        text_render_object->FromRootToContent(args.GetPoint()));
    const auto position = result.position + (result.trailing ? 1 : 0);
    this->select_start_position_ = position;
    log::Debug("TextControlService: Begin to select text, start position: {}.",
               position);
  }
}

void TextControlService::MouseUpHandler(event::MouseButtonEventArgs& args) {
  if (this->select_down_button_.has_value() &&
      this->select_down_button_.value() == args.GetButton()) {
    this->control_->ReleaseMouse();
    this->select_down_button_ = std::nullopt;
    log::Debug("TextControlService: End selecting text.");
  }
}

void TextControlService::LoseFocusHandler(event::FocusChangeEventArgs& args) {
  if (!args.IsWindow()) this->AbortSelection();
}
}  // namespace cru::ui::controls
