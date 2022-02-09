#pragma once
#include "../render/TextRenderObject.h"
#include "cru/platform/gui/InputMethod.h"
#include "cru/platform/gui/TimerHelper.h"
#include "cru/platform/gui/UiApplication.h"
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
  virtual gsl::not_null<render::TextRenderObject*> GetTextRenderObject() = 0;
  // May return nullptr.
  virtual render::ScrollRenderObject* GetScrollRenderObject() = 0;
};

class TextHostControlService;

class TextControlMovePattern : public Object {
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
      std::function<gsl::index(TextHostControlService* service, StringView text,
                               gsl::index current_position)>;

  TextControlMovePattern(String name, helper::ShortcutKeyBind key_bind,
                         MoveFunction move_function)
      : name_(std::move(name)),
        key_bind_(key_bind),
        move_function_(move_function) {}

  CRU_DEFAULT_COPY(TextControlMovePattern)
  CRU_DEFAULT_MOVE(TextControlMovePattern)

  ~TextControlMovePattern() override = default;

 public:
  String GetName() const { return name_; }
  helper::ShortcutKeyBind GetKeyBind() const { return key_bind_; }
  gsl::index Move(TextHostControlService* service, StringView text,
                  gsl::index current_position) const {
    return move_function_(service, text, current_position);
  }

 private:
  String name_;
  helper::ShortcutKeyBind key_bind_;
  MoveFunction move_function_;
};

class CRU_UI_API TextHostControlService : public Object {
  CRU_DEFINE_CLASS_LOG_TAG(u"cru::ui::controls::TextControlService")

 public:
  TextHostControlService(gsl::not_null<Control*> control);

  CRU_DELETE_COPY(TextHostControlService)
  CRU_DELETE_MOVE(TextHostControlService)

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

  String GetText() { return this->text_; }
  StringView GetTextView() { return this->text_; }
  void SetText(String text, bool stop_composition = false);

  void InsertText(gsl::index position, StringView text,
                  bool stop_composition = false);
  void DeleteChar(gsl::index position, bool stop_composition = false);

  // Return the position of deleted character.
  gsl::index DeleteCharPrevious(gsl::index position,
                                bool stop_composition = false);
  void DeleteText(TextRange range, bool stop_composition = false);

  void CancelComposition();

  std::optional<platform::gui::CompositionText> GetCompositionInfo();

  bool IsCaretVisible() { return caret_visible_; }
  void SetCaretVisible(bool visible);

  int GetCaretBlinkDuration() { return caret_blink_duration_; }
  void SetCaretBlinkDuration(int milliseconds);

  gsl::index GetCaretPosition() { return selection_.GetEnd(); }
  TextRange GetSelection() { return selection_; }

  StringView GetSelectedText();

  void SetSelection(gsl::index caret_position);
  void SetSelection(TextRange selection, bool scroll_to_caret = true);

  void SelectAll();

  void ChangeSelectionEnd(gsl::index new_end);
  void AbortSelection();

  void DeleteSelectedText();
  // If some text is selected, then they are deleted first. Then insert text
  // into caret position.
  void ReplaceSelectedText(StringView text);

  void ScrollToCaret();

  void Cut();
  void Copy();
  void Paste();

  gsl::not_null<render::TextRenderObject*> GetTextRenderObject();
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
  gsl::not_null<Control*> control_;
  gsl::not_null<ITextHostControl*> text_host_control_;

  EventRevokerListGuard event_guard_;
  EventRevokerListGuard input_method_context_event_guard_;

  String text_;
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

  bool context_menu_dirty_ = true;
  std::unique_ptr<components::PopupMenu> context_menu_;
};
}  // namespace cru::ui::controls
