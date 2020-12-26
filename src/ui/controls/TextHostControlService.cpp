#include "cru/ui/controls/TextHostControlService.hpp"

#include "../Helper.hpp"
#include "cru/common/Logger.hpp"
#include "cru/common/StringUtil.hpp"
#include "cru/platform/gui/Base.hpp"
#include "cru/platform/gui/Cursor.hpp"
#include "cru/platform/gui/InputMethod.hpp"
#include "cru/platform/gui/Keyboard.hpp"
#include "cru/platform/gui/UiApplication.hpp"
#include "cru/platform/gui/Window.hpp"
#include "cru/ui/Base.hpp"
#include "cru/ui/DebugFlags.hpp"
#include "cru/ui/events/UiEvent.hpp"
#include "cru/ui/helper/ShortcutHub.hpp"
#include "cru/ui/host/WindowHost.hpp"
#include "cru/ui/render/ScrollRenderObject.hpp"
#include "cru/ui/render/TextRenderObject.hpp"

namespace cru::ui::controls {
TextHostControlService::TextHostControlService(gsl::not_null<Control*> control)
    : control_(control),
      text_host_control_(dynamic_cast<ITextHostControl*>(control.get())) {
  SetUpShortcuts();

  SetupOneHandler(&Control::MouseMoveEvent,
                  &TextHostControlService::MouseMoveHandler);
  SetupOneHandler(&Control::MouseDownEvent,
                  &TextHostControlService::MouseDownHandler);
  SetupOneHandler(&Control::MouseUpEvent,
                  &TextHostControlService::MouseUpHandler);
  SetupOneHandler(&Control::GainFocusEvent,
                  &TextHostControlService::GainFocusHandler);
  SetupOneHandler(&Control::LoseFocusEvent,
                  &TextHostControlService::LoseFocusHandler);

  shortcut_hub_.Install(control_);
}

void TextHostControlService::SetEnabled(bool enable) {
  if (enable == this->enable_) return;
  this->enable_ = enable;
  if (enable) {
    if (this->caret_visible_) {
      this->SetupCaret();
    }
    this->control_->SetCursor(
        GetUiApplication()->GetCursorManager()->GetSystemCursor(
            platform::gui::SystemCursorType::IBeam));
  } else {
    this->AbortSelection();
    this->TearDownCaret();
    this->control_->SetCursor(nullptr);
  }
}

void TextHostControlService::SetEditable(bool editable) {
  this->editable_ = editable;
  if (!editable) CancelComposition();
}

void TextHostControlService::SetText(std::u16string text,
                                     bool stop_composition) {
  this->text_ = std::move(text);
  CoerceSelection();
  if (stop_composition) {
    CancelComposition();
  }
  SyncTextRenderObject();
}

void TextHostControlService::InsertText(gsl::index position,
                                        std::u16string_view text,
                                        bool stop_composition) {
  if (!Utf16IsValidInsertPosition(this->text_, position)) {
    log::TagError(log_tag, u"Invalid text insert position.");
    return;
  }
  this->text_.insert(this->text_.cbegin() + position, text.begin(), text.end());
  if (stop_composition) {
    CancelComposition();
  }
  SyncTextRenderObject();
}

void TextHostControlService::DeleteChar(gsl::index position,
                                        bool stop_composition) {
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
gsl::index TextHostControlService::DeleteCharPrevious(gsl::index position,
                                                      bool stop_composition) {
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

void TextHostControlService::DeleteText(TextRange range,
                                        bool stop_composition) {
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
  if (stop_composition) {
    CancelComposition();
  }
  this->SyncTextRenderObject();
}

platform::gui::IInputMethodContext*
TextHostControlService ::GetInputMethodContext() {
  host::WindowHost* host = this->control_->GetWindowHost();
  if (!host) return nullptr;
  platform::gui::INativeWindow* native_window = host->GetNativeWindow();
  if (!native_window) return nullptr;
  return native_window->GetInputMethodContext();
}

void TextHostControlService::CancelComposition() {
  auto input_method_context = GetInputMethodContext();
  if (input_method_context == nullptr) return;
  input_method_context->CancelComposition();
}

std::optional<platform::gui::CompositionText>
TextHostControlService::GetCompositionInfo() {
  auto input_method_context = GetInputMethodContext();
  if (input_method_context == nullptr) return std::nullopt;
  auto composition_info = input_method_context->GetCompositionText();
  if (composition_info.text.empty()) return std::nullopt;
  return composition_info;
}

void TextHostControlService::SetCaretVisible(bool visible) {
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

void TextHostControlService::SetCaretBlinkDuration(int milliseconds) {
  if (this->caret_blink_duration_ == milliseconds) return;

  if (this->enable_ && this->caret_visible_) {
    this->TearDownCaret();
    this->SetupCaret();
  }
}

void TextHostControlService::ScrollToCaret() {
  if (const auto scroll_render_object = this->GetScrollRenderObject()) {
    this->control_->GetWindowHost()->RunAfterLayoutStable(
        [this, scroll_render_object]() {
          const auto caret_rect = this->GetTextRenderObject()->GetCaretRect();
          scroll_render_object->ScrollToContain(caret_rect, Thickness{5.f});
        });
  }
}

gsl::not_null<render::TextRenderObject*>
TextHostControlService::GetTextRenderObject() {
  return this->text_host_control_->GetTextRenderObject();
}

render::ScrollRenderObject* TextHostControlService::GetScrollRenderObject() {
  return this->text_host_control_->GetScrollRenderObject();
}

void TextHostControlService::SetSelection(gsl::index caret_position) {
  this->SetSelection(TextRange{caret_position, 0});
}

void TextHostControlService::SetSelection(TextRange selection,
                                          bool scroll_to_caret) {
  this->selection_ = selection;
  CoerceSelection();
  SyncTextRenderObject();
  if (scroll_to_caret) {
    this->ScrollToCaret();
  }
}

void TextHostControlService::ChangeSelectionEnd(Index new_end) {
  auto selection = GetSelection();
  selection.ChangeEnd(new_end);
  this->SetSelection(selection);
}

void TextHostControlService::AbortSelection() {
  if (this->mouse_move_selecting_) {
    this->control_->ReleaseMouse();
    this->mouse_move_selecting_ = false;
  }
  SetSelection(GetCaretPosition());
}

void TextHostControlService::ReplaceSelectedText(std::u16string_view text) {
  DeleteSelectedText();
  InsertText(GetSelection().GetStart(), text);
  SetSelection(GetSelection().GetStart() + text.size());
}

void TextHostControlService::DeleteSelectedText() {
  this->DeleteText(GetSelection());
  SetSelection(GetSelection().Normalize().GetStart());
}

void TextHostControlService::SetupCaret() {
  const auto application = GetUiApplication();
  this->GetTextRenderObject()->SetDrawCaret(true);
  this->caret_timer_canceler_.Reset(application->SetInterval(
      std::chrono::milliseconds(this->caret_blink_duration_),
      [this] { this->GetTextRenderObject()->ToggleDrawCaret(); }));
}

void TextHostControlService::TearDownCaret() {
  this->caret_timer_canceler_.Reset();
  this->GetTextRenderObject()->SetDrawCaret(false);
}

void TextHostControlService::CoerceSelection() {
  this->selection_ = this->selection_.CoerceInto(0, text_.size());
}

void TextHostControlService::SyncTextRenderObject() {
  const auto text_render_object = this->GetTextRenderObject();
  const auto composition_info = this->GetCompositionInfo();
  if (composition_info) {
    const auto caret_position = GetCaretPosition();
    auto text = this->text_;
    text.insert(caret_position, composition_info->text);
    text_render_object->SetText(text);
    text_render_object->SetCaretPosition(caret_position +
                                         composition_info->selection.GetEnd());
    auto selection = composition_info->selection;
    selection.position += caret_position;
    text_render_object->SetSelectionRange(selection);
  } else {
    text_render_object->SetText(this->text_);
    text_render_object->SetCaretPosition(this->GetCaretPosition());
    text_render_object->SetSelectionRange(this->GetSelection());
  }
}

void TextHostControlService::UpdateInputMethodPosition() {
  if (auto input_method_context = this->GetInputMethodContext()) {
    Point right_bottom =
        this->GetTextRenderObject()->GetTotalOffset() +
        this->GetTextRenderObject()->GetCaretRect().GetRightBottom();
    right_bottom.x += 5;
    right_bottom.y += 5;

    if constexpr (debug_flags::text_service) {
      log::TagDebug(log_tag,
                    u"Calculate input method candidate window position: {}.",
                    right_bottom.ToDebugString());
    }

    input_method_context->SetCandidateWindowPosition(right_bottom);
  }
}

void TextHostControlService::MouseDownHandler(
    event::MouseButtonEventArgs& args) {
  if (IsEnabled()) {
    this->control_->SetFocus();
    if (args.GetButton() == mouse_buttons::left &&
        !this->mouse_move_selecting_) {
      if (!this->control_->CaptureMouse()) return;
      this->mouse_move_selecting_ = true;
      const auto text_render_object = this->GetTextRenderObject();
      const auto result = text_render_object->TextHitTest(
          args.GetPointToContent(text_render_object));
      const auto position = result.position + (result.trailing ? 1 : 0);
      SetSelection(position);
    }
  }
}

void TextHostControlService::MouseUpHandler(event::MouseButtonEventArgs& args) {
  if (args.GetButton() == mouse_buttons::left && mouse_move_selecting_) {
    this->control_->ReleaseMouse();
    this->mouse_move_selecting_ = false;
  }
}

void TextHostControlService::MouseMoveHandler(event::MouseEventArgs& args) {
  if (this->mouse_move_selecting_) {
    const auto text_render_object = this->GetTextRenderObject();
    const auto result = text_render_object->TextHitTest(
        args.GetPointToContent(text_render_object));
    const auto position = result.position + (result.trailing ? 1 : 0);
    ChangeSelectionEnd(position);
  }
}

void TextHostControlService::GainFocusHandler(
    event::FocusChangeEventArgs& args) {
  CRU_UNUSED(args);
  if (editable_) {
    auto input_method_context = GetInputMethodContext();
    if (input_method_context == nullptr) return;
    input_method_context->EnableIME();
    auto sync = [this](std::nullptr_t) {
      this->SyncTextRenderObject();
      ScrollToCaret();
    };
    input_method_context_event_guard_ +=
        input_method_context->CompositionStartEvent()->AddHandler(
            [this](std::nullptr_t) { this->DeleteSelectedText(); });
    input_method_context_event_guard_ +=
        input_method_context->CompositionEvent()->AddHandler(sync);
    input_method_context_event_guard_ +=
        input_method_context->CompositionEndEvent()->AddHandler(sync);
    input_method_context_event_guard_ +=
        input_method_context->TextEvent()->AddHandler(
            [this](const std::u16string_view& text) {
              if (text == u"\b") return;
              this->ReplaceSelectedText(text);
            });

    host::WindowHost* window_host = control_->GetWindowHost();
    if (window_host)
      input_method_context_event_guard_ +=
          window_host->AfterLayoutEvent()->AddHandler(
              [this](auto) { this->UpdateInputMethodPosition(); });
    SetCaretVisible(true);
  }
}

void TextHostControlService::LoseFocusHandler(
    event::FocusChangeEventArgs& args) {
  if (!args.IsWindow()) this->AbortSelection();
  input_method_context_event_guard_.Clear();
  auto input_method_context = GetInputMethodContext();
  if (input_method_context) {
    input_method_context->DisableIME();
  }
  SetCaretVisible(false);
  SyncTextRenderObject();
}

void TextHostControlService::SetUpShortcuts() {
  using platform::gui::KeyCode;
  using platform::gui::KeyModifiers;

  shortcut_hub_.RegisterShortcut(u"Backspace", KeyCode::Backspace, [this] {
    if (!IsEnabled()) return false;
    if (!IsEditable()) return false;
    const auto selection = GetSelection();
    if (selection.count == 0) {
      SetSelection(DeleteCharPrevious(GetCaretPosition()));
    } else {
      this->DeleteSelectedText();
    }
    return true;
  });

  shortcut_hub_.RegisterShortcut(u"Delete", KeyCode::Delete, [this] {
    if (!IsEnabled()) return false;
    if (!IsEditable()) return false;
    const auto selection = GetSelection();
    if (selection.count == 0) {
      DeleteChar(GetCaretPosition());
    } else {
      this->DeleteSelectedText();
    }
    return true;
  });

  shortcut_hub_.RegisterShortcut(u"Left", KeyCode::Left, [this] {
    auto text = this->GetTextView();
    const auto caret = this->GetCaretPosition();
    gsl::index new_position;
    Utf16PreviousCodePoint(text, caret, &new_position);
    this->SetSelection(new_position);
    return true;
  });

  shortcut_hub_.RegisterShortcut(
      u"ShiftLeft", {KeyCode::Left, KeyModifiers::shift}, [this] {
        auto text = this->GetTextView();
        auto selection = this->GetSelection();
        gsl::index new_position;
        Utf16PreviousCodePoint(text, selection.GetEnd(), &new_position);
        selection.ChangeEnd(new_position);
        this->SetSelection(selection);
        return true;
      });

  shortcut_hub_.RegisterShortcut(u"Right", KeyCode::Right, [this] {
    auto text = this->GetTextView();
    const auto caret = this->GetCaretPosition();
    gsl::index new_position;
    Utf16NextCodePoint(text, caret, &new_position);
    this->SetSelection(new_position);
    return true;
  });

  shortcut_hub_.RegisterShortcut(
      u"ShiftRight", {KeyCode::Right, KeyModifiers::shift}, [this] {
        auto text = this->GetTextView();
        auto selection = this->GetSelection();
        gsl::index new_position;
        Utf16NextCodePoint(text, selection.GetEnd(), &new_position);
        selection.ChangeEnd(new_position);
        this->SetSelection(selection);
        return true;
      });
}
}  // namespace cru::ui::controls
