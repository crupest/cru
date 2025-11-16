#pragma once
#include "../render/TextRenderObject.h"
#include "cru/platform/gui/InputMethod.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/ui/DeleteLater.h"
#include "cru/ui/controls/Control.h"
#include "cru/ui/helper/ShortcutHub.h"

#include <functional>
#include <vector>

namespace cru::ui::render {
class TextRenderObject;
class ScrollRenderObject;
}  // namespace cru::ui::render

namespace cru::ui::components {
class PopupMenu;
}

namespace cru::ui::controls {
constexpr int k_default_caret_blink_duration = 500;

struct CRU_UI_API ITextHostControl : virtual Interface {
  virtual render::TextRenderObject* GetTextRenderObject() = 0;
  // May return nullptr.
  virtual render::ScrollRenderObject* GetScrollRenderObject() = 0;
};

class TextHostControlService;

class TextControlMovePattern {
 public:
  static TextControlMovePattern kLeft;
  static TextControlMovePattern kRight;
  static TextControlMovePattern kCtrlLeft;
  static TextControlMovePattern kCtrlRight;
  static TextControlMovePattern kUp;
  static TextControlMovePattern kDown;
  static TextControlMovePattern kHome;
  static TextControlMovePattern kEnd;
  static TextControlMovePattern kCtrlHome;
  static TextControlMovePattern kCtrlEnd;
  static TextControlMovePattern kPageUp;
  static TextControlMovePattern kPageDown;

  static std::vector<TextControlMovePattern> kDefaultPatterns;

  using MoveFunction =
      std::function<Index(TextHostControlService* service,
                          std::string_view text, Index current_position)>;

  TextControlMovePattern(std::string name, helper::ShortcutKeyBind key_bind,
                         MoveFunction move_function)
      : name_(std::move(name)),
        key_bind_(key_bind),
        move_function_(move_function) {}

 public:
  std::string GetName() const { return name_; }
  helper::ShortcutKeyBind GetKeyBind() const { return key_bind_; }
  Index Move(TextHostControlService* service, std::string_view text,
             Index current_position) const {
    return move_function_(service, text, current_position);
  }

 private:
  std::string name_;
  helper::ShortcutKeyBind key_bind_;
  MoveFunction move_function_;
};

class CRU_UI_API TextHostControlService : public Object {
  CRU_DEFINE_CLASS_LOG_TAG("cru::ui::controls::TextControlService")

 public:
  TextHostControlService(Control* control);
  ~TextHostControlService();

 public:
  bool IsEnabled() { return enable_; }
  void SetEnabled(bool enable);

  bool IsEditable() { return this->editable_; }
  void SetEditable(bool editable);

  bool IsContextMenuEnabled() { return this->context_menu_enabled_; }
  void SetContextMenuEnabled(bool enabled);

  bool IsMultiLine() { return this->multi_line_; }
  // If text contains line feed characters, it will be converted to space.
  void SetMultiLine(bool multi_line);

  std::string GetText() { return this->text_; }
  std::string_view GetTextView() { return this->text_; }
  void SetText(std::string text, bool stop_composition = false);

  void InsertText(Index position, std::string_view text,
                  bool stop_composition = false);
  void DeleteChar(Index position, bool stop_composition = false);

  // Return the position of deleted character.
  Index DeleteCharPrevious(Index position, bool stop_composition = false);
  void DeleteText(TextRange range, bool stop_composition = false);

  void CancelComposition();

  std::optional<platform::gui::CompositionText> GetCompositionInfo();

  bool IsCaretVisible() { return caret_visible_; }
  void SetCaretVisible(bool visible);

  int GetCaretBlinkDuration() { return caret_blink_duration_; }
  void SetCaretBlinkDuration(int milliseconds);

  Index GetCaretPosition() { return selection_.GetEnd(); }
  TextRange GetSelection() { return selection_; }

  std::string_view GetSelectedText();

  void SetSelection(Index caret_position);
  void SetSelection(TextRange selection, bool scroll_to_caret = true);

  void SelectAll();

  void ChangeSelectionEnd(Index new_end);
  void AbortSelection();

  void DeleteSelectedText();
  // If some text is selected, then they are deleted first. Then insert text
  // into caret position.
  void ReplaceSelectedText(std::string_view text);

  void ScrollToCaret();

  void Cut();
  void Copy();
  void Paste();

  IEvent<std::nullptr_t>* TextChangeEvent() { return &text_change_event_; }

  render::TextRenderObject* GetTextRenderObject();
  render::ScrollRenderObject* GetScrollRenderObject();

 private:
  // May return nullptr.
  platform::gui::IInputMethodContext* GetInputMethodContext();

  void CoerceSelection();

  void SetupCaret();
  void TearDownCaret();

  void SyncTextRenderObject();

  void UpdateInputMethodPosition();

  template <typename TArgs>
  void SetupOneHandler(events::RoutedEvent<TArgs>* (Control::*event)(),
                       void (TextHostControlService::*handler)(
                           typename events::RoutedEvent<TArgs>::EventArgs)) {
    this->event_guard_ += (this->control_->*event)()->Bubble()->AddHandler(
        std::bind(handler, this, std::placeholders::_1));
  }

  void MouseMoveHandler(events::MouseEventArgs& args);
  void MouseDownHandler(events::MouseButtonEventArgs& args);
  void MouseUpHandler(events::MouseButtonEventArgs& args);
  void GainFocusHandler(events::FocusChangeEventArgs& args);
  void LoseFocusHandler(events::FocusChangeEventArgs& args);

  void SetUpShortcuts();

  enum ContextMenuItem : unsigned {
    kSelectAll = 0b1,
    kCut = 0b10,
    kCopy = 0b100,
    kPaste = 0b1000
  };

  void OpenContextMenu(const Point& position, ContextMenuItem items);

 private:
  Control* control_;
  ITextHostControl* text_host_control_;

  Event<std::nullptr_t> text_change_event_;

  EventHandlerRevokerListGuard event_guard_;
  EventHandlerRevokerListGuard input_method_context_event_guard_;

  std::string text_;
  TextRange selection_;

  bool enable_ = false;
  bool editable_ = false;
  bool multi_line_ = false;
  bool context_menu_enabled_ = true;

  bool caret_visible_ = false;
  platform::gui::TimerAutoCanceler caret_timer_canceler_;
  int caret_blink_duration_ = k_default_caret_blink_duration;

  helper::ShortcutHub shortcut_hub_;

  // true if left mouse is down and selecting
  bool mouse_move_selecting_ = false;

  DeleteLaterPtr<components::PopupMenu> context_menu_;
};
}  // namespace cru::ui::controls
