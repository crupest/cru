#include "cru/ui/controls/TextHostControlService.hpp"

#include "../Helper.hpp"
#include "cru/common/Logger.hpp"
#include "cru/common/String.hpp"
#include "cru/common/StringUtil.hpp"
#include "cru/platform/graphics/Font.hpp"
#include "cru/platform/gui/Base.hpp"
#include "cru/platform/gui/Clipboard.hpp"
#include "cru/platform/gui/Cursor.hpp"
#include "cru/platform/gui/InputMethod.hpp"
#include "cru/platform/gui/Keyboard.hpp"
#include "cru/platform/gui/UiApplication.hpp"
#include "cru/platform/gui/Window.hpp"
#include "cru/ui/Base.hpp"
#include "cru/ui/DebugFlags.hpp"
#include "cru/ui/components/Menu.hpp"
#include "cru/ui/events/UiEvent.hpp"
#include "cru/ui/helper/ShortcutHub.hpp"
#include "cru/ui/host/WindowHost.hpp"
#include "cru/ui/render/ScrollRenderObject.hpp"
#include "cru/ui/render/TextRenderObject.hpp"

#include <memory>

namespace cru::ui::controls {
TextControlMovePattern TextControlMovePattern::kLeft(
    helper::ShortcutKeyBind(platform::gui::KeyCode::Left),
    [](TextHostControlService* service, StringView text,
       gsl::index current_position) {
      Utf16PreviousCodePoint(text, current_position, &current_position);
      return current_position;
    });
TextControlMovePattern TextControlMovePattern::kRight(
    helper::ShortcutKeyBind(platform::gui::KeyCode::Right),
    [](TextHostControlService* service, StringView text,
       gsl::index current_position) {
      Utf16NextCodePoint(text, current_position, &current_position);
      return current_position;
    });
TextControlMovePattern TextControlMovePattern::kCtrlLeft(
    helper::ShortcutKeyBind(platform::gui::KeyCode::Left,
                            platform::gui::KeyModifiers::ctrl),
    [](TextHostControlService* service, StringView text,
       gsl::index current_position) {
      return Utf16PreviousWord(text, current_position);
    });
TextControlMovePattern TextControlMovePattern::kCtrlRight(
    helper::ShortcutKeyBind(platform::gui::KeyCode::Right,
                            platform::gui::KeyModifiers::ctrl),
    [](TextHostControlService* service, StringView text,
       gsl::index current_position) {
      return Utf16NextWord(text, current_position);
    });
TextControlMovePattern TextControlMovePattern::kUp(
    helper::ShortcutKeyBind(platform::gui::KeyCode::Up),
    [](TextHostControlService* service, StringView text,
       gsl::index current_position) {
      auto text_render_object = service->GetTextRenderObject();
      auto rect = text_render_object->TextSinglePoint(current_position, false);
      rect.top -= 0.1f;
      auto result = text_render_object->TextHitTest(rect.GetLeftTop());
      return result.trailing ? result.position + 1 : result.position;
    });
TextControlMovePattern TextControlMovePattern::kDown(
    helper::ShortcutKeyBind(platform::gui::KeyCode::Down),
    [](TextHostControlService* service, StringView text,
       gsl::index current_position) {
      auto text_render_object = service->GetTextRenderObject();
      auto rect = text_render_object->TextSinglePoint(current_position, false);
      rect.top += rect.height + 0.1f;
      auto result = text_render_object->TextHitTest(rect.GetLeftTop());
      return result.trailing ? result.position + 1 : result.position;
    });
TextControlMovePattern TextControlMovePattern::kHome(
    helper::ShortcutKeyBind(platform::gui::KeyCode::Home),
    [](TextHostControlService* service, StringView text,
       gsl::index current_position) {
      return Utf16BackwardUntil(text, current_position,
                                [](char16_t c) { return c == u'\n'; });
    });
TextControlMovePattern TextControlMovePattern::kEnd(
    helper::ShortcutKeyBind(platform::gui::KeyCode::End),
    [](TextHostControlService* service, StringView text,
       gsl::index current_position) {
      return Utf16ForwardUntil(text, current_position,
                               [](char16_t c) { return c == u'\n'; });
    });
TextControlMovePattern TextControlMovePattern::kCtrlHome(
    helper::ShortcutKeyBind(platform::gui::KeyCode::Home,
                            platform::gui::KeyModifiers::ctrl),
    [](TextHostControlService* service, StringView text,
       gsl::index current_position) { return 0; });
TextControlMovePattern TextControlMovePattern::kCtrlEnd(
    helper::ShortcutKeyBind(platform::gui::KeyCode::End,
                            platform::gui::KeyModifiers::ctrl),
    [](TextHostControlService* service, StringView text,
       gsl::index current_position) { return text.size(); });
TextControlMovePattern TextControlMovePattern::kPageUp(
    helper::ShortcutKeyBind(platform::gui::KeyCode::PageUp),
    [](TextHostControlService* service, StringView text,
       gsl::index current_position) {
      // TODO: Implement this.
      return current_position;
    });
TextControlMovePattern TextControlMovePattern::kPageDown(
    helper::ShortcutKeyBind(platform::gui::KeyCode::PageDown),
    [](TextHostControlService* service, StringView text,
       gsl::index current_position) {
      // TODO: Implement this.
      return current_position;
    });

std::vector<TextControlMovePattern> TextControlMovePattern::kDefaultPatterns = {
    TextControlMovePattern::kLeft,     TextControlMovePattern::kRight,
    TextControlMovePattern::kCtrlLeft, TextControlMovePattern::kCtrlRight,
    TextControlMovePattern::kUp,       TextControlMovePattern::kDown,
    TextControlMovePattern::kHome,     TextControlMovePattern::kEnd,
    TextControlMovePattern::kCtrlHome, TextControlMovePattern::kCtrlEnd,
    TextControlMovePattern::kPageUp,   TextControlMovePattern::kPageDown};

TextHostControlService::TextHostControlService(gsl::not_null<Control*> control)
    : control_(control),
      text_host_control_(dynamic_cast<ITextHostControl*>(control.get())) {
  context_menu_ = std::make_unique<components::PopupMenu>();

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

TextHostControlService::~TextHostControlService() {}

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

void TextHostControlService::SetContextMenuEnabled(bool enabled) {
  if (context_menu_enabled_ == enabled) return;

  context_menu_enabled_ = enabled;

  if (!context_menu_enabled_ && context_menu_) {
    context_menu_->Close();
  }
}

void TextHostControlService::SetEditable(bool editable) {
  this->editable_ = editable;
  if (!editable) CancelComposition();
}

void TextHostControlService::SetMultiLine(bool multi_line) {
  this->multi_line_ = multi_line;

  if (!multi_line) {
    auto text = GetText();
    for (auto& c : text) {
      if (c == u'\n') c = u' ';
    }
    SetText(text);
  }
}

void TextHostControlService::SetText(String text, bool stop_composition) {
  this->text_ = std::move(text);
  CoerceSelection();
  if (stop_composition) {
    CancelComposition();
  }
  SyncTextRenderObject();
}

void TextHostControlService::InsertText(gsl::index position, StringView text,
                                        bool stop_composition) {
  if (!Utf16IsValidInsertPosition(this->text_, position)) {
    log::TagError(log_tag, u"Invalid text insert position.");
    return;
  }
  this->text_.insert(this->text_.cbegin() + position, text);
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

StringView TextHostControlService::GetSelectedText() {
  auto selection = this->GetSelection().Normalize();
  return GetTextView().substr(selection.position, selection.count);
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

void TextHostControlService::SelectAll() {
  this->SetSelection(TextRange{0, this->text_.size()});
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

void TextHostControlService::ReplaceSelectedText(StringView text) {
  DeleteSelectedText();
  InsertText(GetSelection().GetStart(), text);
  SetSelection(GetSelection().GetStart() + text.size());
}

void TextHostControlService::DeleteSelectedText() {
  this->DeleteText(GetSelection());
  SetSelection(GetSelection().Normalize().GetStart());
}

void TextHostControlService::Cut() {
  Copy();
  ReplaceSelectedText(StringView{});
}

void TextHostControlService::Copy() {
  auto selected_text = GetSelectedText();
  if (selected_text.size() == 0) return;
  auto clipboard = GetUiApplication()->GetClipboard();
  clipboard->SetText(selected_text.ToString());
}

void TextHostControlService::Paste() {
  auto clipboard = GetUiApplication()->GetClipboard();
  auto text = clipboard->GetText();
  if (text.empty()) return;
  ReplaceSelectedText(text);
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
    text.insert(text.cbegin() + caret_position, composition_info->text);
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
                    right_bottom);
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
        input_method_context->TextEvent()->AddHandler([this](StringView text) {
          if (!multi_line_ && text == u"\n") {
            return;
          }
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
  using platform::gui::kKeyModifierCommand;

  shortcut_hub_.RegisterShortcut(u"Select All",
                                 {KeyCode::A, kKeyModifierCommand}, [this] {
                                   if (IsEnabled()) {
                                     this->SelectAll();
                                     return true;
                                   }
                                   return false;
                                 });

  shortcut_hub_.RegisterShortcut(u"Cut", {KeyCode::X, kKeyModifierCommand},
                                 [this] {
                                   if (IsEnabled() && IsEditable()) {
                                     this->Cut();
                                     return true;
                                   }
                                   return false;
                                 });

  shortcut_hub_.RegisterShortcut(u"Copy", {KeyCode::C, kKeyModifierCommand},
                                 [this] {
                                   if (IsEnabled()) {
                                     this->Copy();
                                     return true;
                                   }
                                   return false;
                                 });

  shortcut_hub_.RegisterShortcut(u"Paste", {KeyCode::V, kKeyModifierCommand},
                                 [this] {
                                   if (IsEnabled() && IsEditable()) {
                                     this->Paste();
                                     return true;
                                   }
                                   return false;
                                 });

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

  for (const auto& pattern : TextControlMovePattern::kDefaultPatterns) {
    shortcut_hub_.RegisterShortcut(u"", pattern.GetKeyBind(), [this, &pattern] {
      auto text = this->GetTextView();
      auto caret = this->GetCaretPosition();
      auto new_position = pattern.Move(this, text, caret);
      this->SetSelection(new_position);
      return true;
    });

    shortcut_hub_.RegisterShortcut(
        u"",
        pattern.GetKeyBind().AddModifier(platform::gui::KeyModifiers::shift),
        [this, &pattern] {
          auto text = this->GetTextView();
          auto caret = this->GetCaretPosition();
          auto new_position = pattern.Move(this, text, caret);
          this->ChangeSelectionEnd(new_position);
          return true;
        });
  }
}

void TextHostControlService::OpenContextMenu(const Point& position,
                                             ContextMenuItem items) {
  context_menu_ = std::make_unique<components::PopupMenu>();
  auto menu = context_menu_->GetMenu();
  if (items & ContextMenuItem::kSelectAll) {
    menu->AddTextItem(u"Select All", [this] { this->SelectAll(); });
  }
  if (items & ContextMenuItem::kCopy) {
    menu->AddTextItem(u"Copy", [this] { this->Copy(); });
  }
  if (items & ContextMenuItem::kCut) {
    menu->AddTextItem(u"Cut", [this] { this->Cut(); });
  }
  if (items & ContextMenuItem::kPaste) {
    menu->AddTextItem(u"Paste", [this] { this->Paste(); });
  }
  context_menu_->SetPosition(position);
  context_menu_->Show();
}

}  // namespace cru::ui::controls
