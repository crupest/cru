#include "cru/ui/controls/TextHostControlService.h"

#include "cru/base/Base.h"
#include "cru/base/StringUtil.h"
#include "cru/base/log/Logger.h"
#include "cru/platform/gui/Clipboard.h"
#include "cru/platform/gui/Cursor.h"
#include "cru/platform/gui/Input.h"
#include "cru/platform/gui/InputMethod.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/platform/gui/Window.h"
#include "cru/ui/Base.h"
#include "cru/ui/DebugFlags.h"
#include "cru/ui/components/Menu.h"
#include "cru/ui/controls/ControlHost.h"
#include "cru/ui/helper/ShortcutHub.h"
#include "cru/ui/render/ScrollRenderObject.h"
#include "cru/ui/render/TextRenderObject.h"

#include <memory>

namespace cru::ui::controls {
using namespace cru::string;
using platform::gui::IUiApplication;

TextControlMovePattern TextControlMovePattern::kLeft(
    "Left", helper::ShortcutKeyBind(platform::gui::KeyCode::Left),
    [](TextHostControlService* service, std::string_view text,
       Index current_position) {
      CRU_UNUSED(service)
      Utf8PreviousCodePoint(text, current_position, &current_position);
      return current_position;
    });
TextControlMovePattern TextControlMovePattern::kRight(
    "Right", helper::ShortcutKeyBind(platform::gui::KeyCode::Right),
    [](TextHostControlService* service, std::string_view text,
       Index current_position) {
      CRU_UNUSED(service)
      Utf8NextCodePoint(text, current_position, &current_position);
      return current_position;
    });
TextControlMovePattern TextControlMovePattern::kCtrlLeft(
    "Ctrl+Left(Previous Word)",
    helper::ShortcutKeyBind(platform::gui::KeyCode::Left,
                            platform::gui::KeyModifiers::Ctrl),
    [](TextHostControlService* service, std::string_view text,
       Index current_position) {
      CRU_UNUSED(service)
      return Utf8PreviousWord(text, current_position);
    });
TextControlMovePattern TextControlMovePattern::kCtrlRight(
    "Ctrl+Right(Next Word)",
    helper::ShortcutKeyBind(platform::gui::KeyCode::Right,
                            platform::gui::KeyModifiers::Ctrl),
    [](TextHostControlService* service, std::string_view text,
       Index current_position) {
      CRU_UNUSED(service)
      return Utf8NextWord(text, current_position);
    });
TextControlMovePattern TextControlMovePattern::kUp(
    "Up", helper::ShortcutKeyBind(platform::gui::KeyCode::Up),
    [](TextHostControlService* service, std::string_view text,
       Index current_position) {
      CRU_UNUSED(text)
      auto text_render_object = service->GetTextRenderObject();
      auto rect = text_render_object->TextSinglePoint(current_position, false);
      rect.top -= 0.1f;
      auto result = text_render_object->TextHitTest(rect.GetLeftTop());
      return result.position_with_trailing;
    });
TextControlMovePattern TextControlMovePattern::kDown(
    "Down", helper::ShortcutKeyBind(platform::gui::KeyCode::Down),
    [](TextHostControlService* service, std::string_view text,
       Index current_position) {
      CRU_UNUSED(text)
      auto text_render_object = service->GetTextRenderObject();
      auto rect = text_render_object->TextSinglePoint(current_position, false);
      rect.top += rect.height + 0.1f;
      auto result = text_render_object->TextHitTest(rect.GetLeftTop());
      return result.position_with_trailing;
    });
TextControlMovePattern TextControlMovePattern::kHome(
    "Home(Line Begin)", helper::ShortcutKeyBind(platform::gui::KeyCode::Home),
    [](TextHostControlService* service, std::string_view text,
       Index current_position) {
      CRU_UNUSED(service)
      return Utf8BackwardUntil(text, current_position,
                               [](CodePoint c) { return c == u'\n'; });
    });
TextControlMovePattern TextControlMovePattern::kEnd(
    "End(Line End)", helper::ShortcutKeyBind(platform::gui::KeyCode::End),
    [](TextHostControlService* service, std::string_view text,
       Index current_position) {
      CRU_UNUSED(service)
      return Utf8ForwardUntil(text, current_position,
                              [](CodePoint c) { return c == u'\n'; });
    });
TextControlMovePattern TextControlMovePattern::kCtrlHome(
    "Ctrl+Home(Document Begin)",
    helper::ShortcutKeyBind(platform::gui::KeyCode::Home,
                            platform::gui::KeyModifiers::Ctrl),
    [](TextHostControlService* service, std::string_view text,
       Index current_position) {
      CRU_UNUSED(service)
      CRU_UNUSED(text)
      CRU_UNUSED(current_position)
      return 0;
    });
TextControlMovePattern TextControlMovePattern::kCtrlEnd(
    "Ctrl+End(Document End)",
    helper::ShortcutKeyBind(platform::gui::KeyCode::End,
                            platform::gui::KeyModifiers::Ctrl),
    [](TextHostControlService* service, std::string_view text,
       Index current_position) {
      CRU_UNUSED(service)
      CRU_UNUSED(text)
      CRU_UNUSED(current_position)
      return text.size();
    });
TextControlMovePattern TextControlMovePattern::kPageUp(
    "PageUp", helper::ShortcutKeyBind(platform::gui::KeyCode::PageUp),
    [](TextHostControlService* service, std::string_view text,
       Index current_position) {
      CRU_UNUSED(service)
      CRU_UNUSED(text)
      // TODO: Implement this.
      return current_position;
    });
TextControlMovePattern TextControlMovePattern::kPageDown(
    "PageDown", helper::ShortcutKeyBind(platform::gui::KeyCode::PageDown),
    [](TextHostControlService* service, std::string_view text,
       Index current_position) {
      CRU_UNUSED(service)
      CRU_UNUSED(text)
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

TextHostControlService::TextHostControlService(Control* control)
    : control_(control),
      text_host_control_(dynamic_cast<ITextHostControl*>(control)) {
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
        IUiApplication::GetInstance()->GetCursorManager()->GetSystemCursor(
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

void TextHostControlService::SetText(std::string text, bool stop_composition) {
  this->text_ = std::move(text);
  CoerceSelection();
  if (stop_composition) {
    CancelComposition();
  }
  SyncTextRenderObject();
  text_change_event_.Raise(nullptr);
}

void TextHostControlService::InsertText(Index position, std::string_view text,
                                        bool stop_composition) {
  if (!Utf8IsValidInsertPosition(this->text_, position)) {
    CruLogError(kLogTag, "Invalid text insert position.");
    return;
  }
  this->text_.insert(this->text_.cbegin() + position, text.cbegin(),
                     text.cend());
  if (stop_composition) {
    CancelComposition();
  }
  SyncTextRenderObject();
  text_change_event_.Raise(nullptr);
}

void TextHostControlService::DeleteChar(Index position, bool stop_composition) {
  if (!Utf8IsValidInsertPosition(this->text_, position)) {
    CruLogError(kLogTag, "Invalid text delete position {}.", position);
    return;
  }
  if (position == static_cast<Index>(this->text_.size())) return;
  Index next;
  Utf8NextCodePoint(this->text_, position, &next);
  this->DeleteText(TextRange::FromTwoSides(position, next), stop_composition);
}

// Return the position of deleted character.
Index TextHostControlService::DeleteCharPrevious(Index position,
                                                 bool stop_composition) {
  if (!Utf8IsValidInsertPosition(this->text_, position)) {
    CruLogError(kLogTag, "Invalid text delete position {}.", position);
    return 0;
  }
  if (position == 0) return 0;
  Index previous;
  Utf8PreviousCodePoint(this->text_, position, &previous);
  this->DeleteText(TextRange::FromTwoSides(previous, position),
                   stop_composition);
  return previous;
}

void TextHostControlService::DeleteText(TextRange range,
                                        bool stop_composition) {
  if (range.count == 0) return;
  range = range.Normalize();
  if (!Utf8IsValidInsertPosition(this->text_, range.GetStart())) {
    CruLogError(kLogTag, "Invalid text delete start position {}.",
                range.GetStart());
    return;
  }
  if (!Utf8IsValidInsertPosition(this->text_, range.GetEnd())) {
    CruLogError(kLogTag, "Invalid text delete end position {}.",
                range.GetEnd());
    return;
  }
  this->text_.erase(this->text_.cbegin() + range.GetStart(),
                    this->text_.cbegin() + range.GetEnd());
  this->CoerceSelection();
  if (stop_composition) {
    CancelComposition();
  }
  this->SyncTextRenderObject();
  text_change_event_.Raise(nullptr);
}

platform::gui::IInputMethodContext*
TextHostControlService ::GetInputMethodContext() {
  auto host = this->control_->GetControlHost();
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
    scroll_to_caret_timer_canceler_.Reset(
        platform::gui::IUiApplication::GetInstance()->SetImmediate(
            [this, scroll_render_object] {
              const auto caret_rect =
                  this->GetTextRenderObject()->GetCaretRect();
              scroll_render_object->ScrollToContain(caret_rect, Thickness{5.f});
            }));
  }
}

render::TextRenderObject* TextHostControlService::GetTextRenderObject() {
  return this->text_host_control_->GetTextRenderObject();
}

render::ScrollRenderObject* TextHostControlService::GetScrollRenderObject() {
  return this->text_host_control_->GetScrollRenderObject();
}

std::string_view TextHostControlService::GetSelectedText() {
  auto selection = this->GetSelection().Normalize();
  return GetTextView().substr(selection.position, selection.count);
}

void TextHostControlService::SetSelection(Index caret_position) {
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
  this->SetSelection(TextRange{0, static_cast<Index>(this->text_.size())});
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

void TextHostControlService::ReplaceSelectedText(std::string_view text) {
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
  ReplaceSelectedText(std::string_view{});
}

void TextHostControlService::Copy() {
  auto selected_text = GetSelectedText();
  if (selected_text.size() == 0) return;
  auto clipboard = IUiApplication::GetInstance()->GetClipboard();
  clipboard->SetText(std::string(selected_text));
}

void TextHostControlService::Paste() {
  auto clipboard = IUiApplication::GetInstance()->GetClipboard();
  auto text = clipboard->GetText();
  if (text.empty()) return;
  ReplaceSelectedText(text);
}

void TextHostControlService::SetupCaret() {
  const auto application = IUiApplication::GetInstance();
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
    text.insert(text.cbegin() + caret_position, composition_info->text.cbegin(),
                composition_info->text.cend());
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
      CruLogDebug(kLogTag,
                  "Calculate input method candidate window position: {}.",
                  right_bottom);
    }

    input_method_context->SetCandidateWindowPosition(right_bottom);
  }
}

void TextHostControlService::MouseDownHandler(
    events::MouseButtonEventArgs& args) {
  if (IsEnabled()) {
    this->control_->SetFocus();
    if (args.GetButton() == MouseButtons::Left &&
        !this->mouse_move_selecting_) {
      if (!this->control_->CaptureMouse()) return;
      this->mouse_move_selecting_ = true;
      const auto text_render_object = this->GetTextRenderObject();
      const auto result = text_render_object->TextHitTest(
          args.GetPointToContent(text_render_object));
      const auto position = result.position_with_trailing;
      SetSelection(position);
      args.SetHandled(true);
    } else if (args.GetButton() == MouseButtons::Right) {
      // TODO: Finish context menu logic here.

      const Point p = args.GetPointToContent(GetTextRenderObject());
      if (GetSelection().count != 0) {
        auto selected_area =
            GetTextRenderObject()->TextRangeRect(GetSelection());

        bool inside = false;

        for (const auto& rect : selected_area) {
          if (rect.IsPointInside(p)) {
            inside = true;
            break;
          }
        }

        if (inside) {
          ContextMenuItem items = ContextMenuItem(kSelectAll | kCopy);
          if (IsEditable()) {
            items = ContextMenuItem(items | kCut | kPaste);
          }

          this->OpenContextMenu(args.GetPointOfScreen(), items);
          args.SetHandled(true);
          return;
        }
      }

      const auto text_render_object = this->GetTextRenderObject();
      const auto result = text_render_object->TextHitTest(
          args.GetPointToContent(text_render_object));
      const auto position = result.position_with_trailing;
      SetSelection(position);

      ContextMenuItem items = ContextMenuItem::kSelectAll;
      if (IsEditable()) {
        items = ContextMenuItem(items | ContextMenuItem::kPaste);
      }

      OpenContextMenu(args.GetPointOfScreen(), items);

      args.SetHandled(true);
    }
  }
}

void TextHostControlService::MouseUpHandler(
    events::MouseButtonEventArgs& args) {
  if (args.GetButton() == MouseButtons::Left && mouse_move_selecting_) {
    this->control_->ReleaseMouse();
    this->mouse_move_selecting_ = false;
    args.SetHandled();
  }
}

void TextHostControlService::MouseMoveHandler(events::MouseEventArgs& args) {
  if (this->mouse_move_selecting_) {
    const auto text_render_object = this->GetTextRenderObject();
    const auto result = text_render_object->TextHitTest(
        args.GetPointToContent(text_render_object));
    const auto position = result.position_with_trailing;
    ChangeSelectionEnd(position);
    args.SetHandled();
  }
}

void TextHostControlService::GainFocusHandler(
    events::FocusChangeEventArgs& args) {
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
            [this](std::string_view text) {
              if (!multi_line_ && text == "\n") {
                return;
              }
              this->ReplaceSelectedText(text);
            });

    auto host = control_->GetControlHost();
    if (host)
      input_method_context_event_guard_ += host->AfterLayoutEvent()->AddHandler(
          [this](auto) { this->UpdateInputMethodPosition(); });
    SetCaretVisible(true);
  }
}

void TextHostControlService::LoseFocusHandler(
    events::FocusChangeEventArgs& args) {
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

  shortcut_hub_.RegisterShortcut("Select All",
                                 {KeyCode::A, kKeyModifierCommand}, [this] {
                                   if (IsEnabled()) {
                                     this->SelectAll();
                                     return true;
                                   }
                                   return false;
                                 });

  shortcut_hub_.RegisterShortcut("Cut", {KeyCode::X, kKeyModifierCommand},
                                 [this] {
                                   if (IsEnabled() && IsEditable()) {
                                     this->Cut();
                                     return true;
                                   }
                                   return false;
                                 });

  shortcut_hub_.RegisterShortcut("Copy", {KeyCode::C, kKeyModifierCommand},
                                 [this] {
                                   if (IsEnabled()) {
                                     this->Copy();
                                     return true;
                                   }
                                   return false;
                                 });

  shortcut_hub_.RegisterShortcut("Paste", {KeyCode::V, kKeyModifierCommand},
                                 [this] {
                                   if (IsEnabled() && IsEditable()) {
                                     this->Paste();
                                     return true;
                                   }
                                   return false;
                                 });

  shortcut_hub_.RegisterShortcut("Backspace", KeyCode::Backspace, [this] {
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

  shortcut_hub_.RegisterShortcut("Delete", KeyCode::Delete, [this] {
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
    auto name = pattern.GetName();
    shortcut_hub_.RegisterShortcut(
        "Move " + name, pattern.GetKeyBind(), [this, &pattern] {
          auto text = this->GetTextView();
          auto caret = this->GetCaretPosition();
          auto new_position = pattern.Move(this, text, caret);
          this->SetSelection(new_position);
          return true;
        });

    shortcut_hub_.RegisterShortcut(
        "Move And Select " + name,
        pattern.GetKeyBind().AddModifier(platform::gui::KeyModifiers::Shift),
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
  CruLogDebug(kLogTag, "Open context menu.");
  if (!context_menu_) {
    context_menu_.reset(new components::PopupMenu());
  }
  auto menu = context_menu_->GetMenu();
  menu->ClearItems();
  if (items & ContextMenuItem::kSelectAll) {
    menu->AddTextItem("Select All", [this] { this->SelectAll(); });
  }
  if (items & ContextMenuItem::kCopy) {
    menu->AddTextItem("Copy", [this] { this->Copy(); });
  }
  if (items & ContextMenuItem::kCut) {
    menu->AddTextItem("Cut", [this] { this->Cut(); });
  }
  if (items & ContextMenuItem::kPaste) {
    menu->AddTextItem("Paste", [this] { this->Paste(); });
  }
  context_menu_->Show();
  context_menu_->SetPosition(position);
}

}  // namespace cru::ui::controls
