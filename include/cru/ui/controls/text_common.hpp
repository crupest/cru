#pragma once
#include "../base.hpp"

#include "cru/ui/ui_event.hpp"

#include <functional>
#include <optional>

namespace cru::platform::graph {
struct IBrush;
}

namespace cru::ui::render {
class TextRenderObject;
class CanvasRenderObject;
}  // namespace cru::ui::render

namespace cru::ui::controls {

struct ITextControl : virtual Interface {
  virtual render::TextRenderObject* GetTextRenderObject() = 0;
  virtual render::CanvasRenderObject* GetCaretRenderObject() = 0;
  virtual platform::graph::IBrush* GetCaretBrush() = 0;
};

class TextControlService : public Object {
 public:
  TextControlService(Control* control, ITextControl* text_control);

  CRU_DELETE_COPY(TextControlService)
  CRU_DELETE_MOVE(TextControlService)

  ~TextControlService();

 public:
  int GetCaretPosition() { return caret_position_; }
  void SetCaretPosition(int position) { caret_position_ = position; }

  bool IsCaretVisible() { return caret_visible_; }
  void SetCaretVisible(bool visible);

  // please set correct offset before calling this
  void DrawCaret(platform::graph::IPainter* painter);

 private:
  void SetupCaretTimer();
  void TearDownCaretTimer();

 private:
  Control* control_;
  ITextControl* text_control_;
  std::vector<EventRevokerGuard> event_revoker_guards_;

  bool caret_visible_ = false;
  int caret_position_ = 0;
  unsigned long caret_timer_tag_;
  // this is used for blinking of caret
  bool caret_show_ = true;

  // nullopt means not selecting
  std::optional<MouseButton> select_down_button_;

  // before the char
  int select_start_position_;
};
}  // namespace cru::ui::controls
