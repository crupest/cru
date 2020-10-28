#pragma once
#include "../Helper.hpp"
#include "cru/common/Logger.hpp"
#include "cru/common/StringUtil.hpp"
#include "cru/platform/graph/Font.hpp"
#include "cru/platform/graph/Painter.hpp"
#include "cru/platform/native/InputMethod.hpp"
#include "cru/platform/native/UiApplication.hpp"
#include "cru/platform/native/Window.hpp"
#include "cru/ui/Base.hpp"
#include "cru/ui/Control.hpp"
#include "cru/ui/DebugFlags.hpp"
#include "cru/ui/ShortcutHub.hpp"
#include "cru/ui/UiEvent.hpp"
#include "cru/ui/UiHost.hpp"
#include "cru/ui/render/CanvasRenderObject.hpp"
#include "cru/ui/render/ScrollRenderObject.hpp"
#include "cru/ui/render/TextRenderObject.hpp"

#include <string>

namespace cru::ui::controls {
constexpr int k_default_caret_blink_duration = 500;

// TControl should inherits `Control` and has following methods:
// ```
// gsl::not_null<render::TextRenderObject*> GetTextRenderObject();
// render::ScrollRenderObject* GetScrollRenderObject();
// ```
template <typename TControl>
class TextControlService : public Object {
  CRU_DEFINE_CLASS_LOG_TAG(u"cru::ui::controls::TextControlService")

 public:
  TextControlService(gsl::not_null<TControl*> control) : control_(control) {}

  CRU_DELETE_COPY(TextControlService)
  CRU_DELETE_MOVE(TextControlService)

  ~TextControlService() = default;

 public:
  bool IsEnabled() { return enable_; }

  void SetEnabled(bool enable) {
    if (enable == this->enable_) return;
    this->enable_ = enable;
    if (enable) {
      this->SetUpHandlers();
      if (this->caret_visible_) {
        this->SetupCaret();
      }
    } else {
      this->AbortSelection();
      this->TearDownHandlers();
      this->TearDownCaret();
    }
  }

  bool IsEditable() { return this->editable_; }

  void SetEditable(bool editable) {
    this->editable_ = editable;
    this->input_method_context_.reset();
  }

  std::u16string GetText() { return this->text_; }
  std::u16string_view GetTextView() { return this->text_; }
  void SetText(std::u16string text, bool stop_composition = false) {
    this->text_ = std::move(text);
    CoerceSelection();
    if (stop_composition && this->input_method_context_) {
      this->input_method_context_->CancelComposition();
    }
    SyncTextRenderObject();
  }

  void InsertText(gsl::index position, std::u16string_view text,
                  bool stop_composition = false) {
    if (!Utf16IsValidInsertPosition(this->text_, position)) {
      log::TagError(log_tag, u"Invalid text insert position.");
      return;
    }
    this->text_.insert(this->text_.cbegin() + position, text.begin(),
                       text.end());
    if (stop_composition && this->input_method_context_) {
      this->input_method_context_->CancelComposition();
    }
    SyncTextRenderObject();
  }

  void DeleteChar(gsl::index position, bool stop_composition = false) {
    if (!Utf16IsValidInsertPosition(this->text_, position)) {
      log::TagError(log_tag, u"Invalid text delete position.");
      return;
    }
    if (position == static_cast<gsl::index>(this->text_.size())) return;
    Index next;
    Utf16NextCodePoint(this->text_, position, &next);
    this->DeleteText(TextRange::FromTwoSides(position, next), stop_composition);
  }

  // Return the position of deleted character.
  gsl::index DeleteCharPrevious(gsl::index position,
                                bool stop_composition = false) {
    if (!Utf16IsValidInsertPosition(this->text_, position)) {
      log::TagError(log_tag, u"Invalid text delete position.");
      return 0;
    }
    if (position == 0) return 0;
    Index previous;
    Utf16PreviousCodePoint(this->text_, position, &previous);
    this->DeleteText(TextRange::FromTwoSides(previous, position),
                     stop_composition);
    return previous;
  }

  void DeleteText(TextRange range, bool stop_composition = false) {
    if (range.count == 0) return;
    range = range.Normalize();
    if (!Utf16IsValidInsertPosition(this->text_, range.GetStart())) {
      log::TagError(log_tag, u"Invalid text delete start position.");
      return;
    }
    if (!Utf16IsValidInsertPosition(this->text_, range.GetStart())) {
      log::TagError(log_tag, u"Invalid text delete end position.");
      return;
    }
    this->text_.erase(this->text_.cbegin() + range.GetStart(),
                      this->text_.cbegin() + range.GetEnd());
    this->CoerceSelection();
    if (stop_composition && this->input_method_context_) {
      this->input_method_context_->CancelComposition();
    }
    this->SyncTextRenderObject();
  }

  std::optional<platform::native::CompositionText> GetCompositionInfo() {
    if (this->input_method_context_ == nullptr) return std::nullopt;
    auto composition_info = this->input_method_context_->GetCompositionText();
    if (composition_info.text.empty()) return std::nullopt;
    return composition_info;
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

  gsl::index GetCaretPosition() { return selection_.GetEnd(); }

  TextRange GetSelection() { return selection_; }

  void SetSelection(gsl::index caret_position) {
    this->SetSelection(TextRange{caret_position, 0});
  }

  void SetSelection(TextRange selection, bool scroll_to_caret = true) {
    this->selection_ = selection;
    CoerceSelection();
    SyncTextRenderObject();
    if (scroll_to_caret) {
      this->ScrollToCaret();
    }
  }

  void DeleteSelectedText() {
    this->DeleteText(GetSelection());
    SetSelection(GetSelection().GetStart());
  }

  // If some text is selected, then they are deleted first. Then insert text
  // into caret position.
  void ReplaceSelectedText(std::u16string_view text) {
    DeleteSelectedText();
    InsertText(GetSelection().GetStart(), text);
    SetSelection(GetSelection().GetStart() + text.size());
  }

  void ScrollToCaret() {
    this->control_->GetUiHost()->RunAfterLayoutStable([this]() {
      const auto caret_rect = this->GetTextRenderObject()->GetCaretRect();
      this->GetScrollRenderObject()->ScrollToContain(caret_rect,
                                                     Thickness{5.f});
    });
  }

 private:
  void CoerceSelection() {
    this->selection_ = this->selection_.CoerceInto(0, text_.size());
  }

  void AbortSelection() {
    if (this->select_down_button_.has_value()) {
      this->control_->ReleaseMouse();
      this->select_down_button_ = std::nullopt;
    }
    this->GetTextRenderObject()->SetSelectionRange(std::nullopt);
  }

  void SetupCaret() {
    const auto application = GetUiApplication();
    this->GetTextRenderObject()->SetDrawCaret(true);
    this->caret_timer_canceler_.Reset(application->SetInterval(
        std::chrono::milliseconds(this->caret_blink_duration_),
        [this] { this->GetTextRenderObject()->ToggleDrawCaret(); }));
  }

  void TearDownCaret() {
    this->caret_timer_canceler_.Reset();
    this->GetTextRenderObject()->SetDrawCaret(false);
  }

  void SyncTextRenderObject() {
    const auto text_render_object = this->GetTextRenderObject();
    const auto composition_info = this->GetCompositionInfo();
    if (composition_info) {
      const auto caret_position = GetCaretPosition();
      auto text = this->text_;
      text.insert(caret_position, composition_info->text);
      text_render_object->SetText(text);
      text_render_object->SetCaretPosition(
          caret_position + composition_info->selection.GetEnd());
      auto selection = composition_info->selection;
      selection.position += caret_position;
      text_render_object->SetSelectionRange(selection);
    } else {
      text_render_object->SetText(this->text_);
      text_render_object->SetCaretPosition(this->GetCaretPosition());
      text_render_object->SetSelectionRange(this->GetSelection());
    }
  }

  void StartSelection(Index start) {
    SetSelection(start);
    if constexpr (debug_flags::text_service)
      log::TagDebug(log_tag, u"Text selection started, position: {}.", start);
  }

  void UpdateSelection(Index new_end) {
    auto selection = GetSelection();
    selection.AdjustEnd(new_end);
    this->SetSelection(selection);
    if constexpr (debug_flags::text_service)
      log::TagDebug(log_tag, u"Text selection updated, range: {}, {}.",
                    selection.GetStart(), selection.GetEnd());
  }

  template <typename TArgs>
  void SetupOneHandler(event::RoutedEvent<TArgs>* (Control::*event)(),
                       void (TextControlService::*handler)(
                           typename event::RoutedEvent<TArgs>::EventArgs)) {
    this->event_guard_ += (this->control_->*event)()->Direct()->AddHandler(
        std::bind(handler, this, std::placeholders::_1));
  }

  void SetUpHandlers() {
    Expects(event_guard_.IsEmpty());

    SetupOneHandler(&Control::MouseMoveEvent,
                    &TextControlService::MouseMoveHandler);
    SetupOneHandler(&Control::MouseDownEvent,
                    &TextControlService::MouseDownHandler);
    SetupOneHandler(&Control::MouseUpEvent,
                    &TextControlService::MouseUpHandler);
    SetupOneHandler(&Control::KeyDownEvent,
                    &TextControlService::KeyDownHandler);
    SetupOneHandler(&Control::GainFocusEvent,
                    &TextControlService::GainFocusHandler);
    SetupOneHandler(&Control::LoseFocusEvent,
                    &TextControlService::LoseFocusHandler);

    shortcut_hub_.Install(control_);
  }

  void TearDownHandlers() {
    event_guard_.Clear();
    shortcut_hub_.Uninstall();
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
    this->control_->RequestFocus();
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

  void KeyDownHandler(event::KeyEventArgs& args) {
    const auto key_code = args.GetKeyCode();
    using cru::platform::native::KeyCode;
    using cru::platform::native::KeyModifiers;

    switch (key_code) {
      case KeyCode::Backspace: {
        if (!IsEditable()) return;
        const auto selection = GetSelection();
        if (selection.count == 0) {
          SetSelection(DeleteCharPrevious(GetCaretPosition()));
        } else {
          this->DeleteSelectedText();
        }
      } break;
      case KeyCode::Delete: {
        if (!IsEditable()) return;
        const auto selection = GetSelection();
        if (selection.count == 0) {
          DeleteChar(GetCaretPosition());
        } else {
          this->DeleteSelectedText();
        }
      } break;
      case KeyCode::Left: {
        const auto key_modifier = args.GetKeyModifier();
        const bool shift = key_modifier & KeyModifiers::shift;
        auto text = this->GetTextView();
        if (shift) {
          auto selection = this->GetSelection();
          gsl::index new_position;
          Utf16PreviousCodePoint(text, selection.GetEnd(), &new_position);
          selection.AdjustEnd(new_position);
          this->SetSelection(selection);
        } else {
          const auto caret = this->GetCaretPosition();
          gsl::index new_position;
          Utf16PreviousCodePoint(text, caret, &new_position);
          this->SetSelection(new_position);
        }
      } break;
      case KeyCode::Right: {
        const auto key_modifier = args.GetKeyModifier();
        const bool shift = key_modifier & KeyModifiers::shift;
        auto text = this->GetTextView();
        if (shift) {
          auto selection = this->GetSelection();
          gsl::index new_position;
          Utf16NextCodePoint(text, selection.GetEnd(), &new_position);
          selection.AdjustEnd(new_position);
          this->SetSelection(selection);
        } else {
          const auto caret = this->GetCaretPosition();
          gsl::index new_position;
          Utf16NextCodePoint(text, caret, &new_position);
          this->SetSelection(new_position);
        }
      } break;
      default:
        break;
    }
  }

  void GainFocusHandler(event::FocusChangeEventArgs& args) {
    CRU_UNUSED(args);
    if (editable_) {
      UiHost* ui_host = this->control_->GetUiHost();
      auto window = ui_host->GetNativeWindowResolver()->Resolve();
      if (window == nullptr) return;
      input_method_context_ =
          GetUiApplication()->GetInputMethodManager()->GetContext(window);
      input_method_context_->EnableIME();
      auto sync = [this](std::nullptr_t) {
        this->SyncTextRenderObject();
        ScrollToCaret();
      };
      input_method_context_->CompositionStartEvent()->AddHandler(
          [this](std::nullptr_t) { this->DeleteSelectedText(); });
      input_method_context_->CompositionEvent()->AddHandler(sync);
      input_method_context_->CompositionEndEvent()->AddHandler(sync);
      input_method_context_->TextEvent()->AddHandler(
          [this](const std::u16string_view& text) {
            if (text == u"\b") return;
            this->ReplaceSelectedText(text);
          });
    }
  }

  void LoseFocusHandler(event::FocusChangeEventArgs& args) {
    if (!args.IsWindow()) this->AbortSelection();
    if (input_method_context_) {
      input_method_context_->DisableIME();
      input_method_context_.reset();
    }
    SyncTextRenderObject();
  }

 private:
  gsl::not_null<TControl*> control_;
  EventRevokerListGuard event_guard_;

  std::u16string text_;
  TextRange selection_;

  bool enable_ = false;
  bool editable_ = false;

  bool caret_visible_ = false;
  platform::native::TimerAutoCanceler caret_timer_canceler_;
  int caret_blink_duration_ = k_default_caret_blink_duration;

  ShortcutHub shortcut_hub_;

  // nullopt means not selecting
  std::optional<MouseButton> select_down_button_;

  std::unique_ptr<platform::native::IInputMethodContext> input_method_context_;
};  // namespace cru::ui::controls
}  // namespace cru::ui::controls
