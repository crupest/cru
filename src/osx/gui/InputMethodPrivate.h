#pragma once
#include "cru/osx/gui/InputMethod.hpp"

#include <AppKit/AppKit.h>

namespace cru::platform::gui::osx {
namespace details {
class OsxInputMethodContextPrivate {
  friend OsxInputMethodContext;

 public:
  OsxInputMethodContextPrivate(OsxInputMethodContext* input_method_context,
                               OsxWindow* window);

  CRU_DELETE_COPY(OsxInputMethodContextPrivate)
  CRU_DELETE_MOVE(OsxInputMethodContextPrivate)

  ~OsxInputMethodContextPrivate();

  void SetCompositionText(CompositionText composition_text) {
    composition_text_ = std::move(composition_text);
  }

  void RaiseCompositionStartEvent();
  void RaiseCompositionEndEvent();
  void RaiseCompositionEvent();
  void RaiseTextEvent(StringView text);

  Point GetCandidateWindowPosition() const { return candidate_window_point_; }
  void SetCandidateWindowPosition(const Point& p) {
    candidate_window_point_ = p;
  }

  Range GetSelectionRange() const { return selection_range_; }
  void SetSelectionRange(Range selection_range) {
    selection_range_ = selection_range;
  }

  void PerformSel(SEL sel);

  void Activate();
  void Deactivate();

 private:
  OsxWindow* window_;

  CompositionText composition_text_;

  Range selection_range_;

  OsxInputMethodContext* input_method_context_;

  // On Osx, this is the text lefttop point on screen.
  Point candidate_window_point_;

  Event<std::nullptr_t> composition_start_event_;
  Event<std::nullptr_t> composition_event_;
  Event<std::nullptr_t> composition_end_event_;
  Event<StringView> text_event_;

  bool is_enabled_ = false;
};
}  // namespace details
}  // namespace cru::platform::gui::osx
