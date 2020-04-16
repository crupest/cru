#pragma once
#include "../resource.hpp"
#include "base.hpp"
#include "cru/common/event.hpp"

#include <memory>
#include <vector>

namespace cru::platform::native {
struct CompositionUnderline {
  int start;
  int end;
};

struct CompositionText {
  std::string text;
  std::vector<CompositionUnderline> underlines;
  int caret_position;
};

struct IInputMethodContext : virtual INativeResource {
  // Return true if you should draw composition text manually. Return false if
  // system will take care of that for you.
  virtual bool ShouldManuallyDrawCompositionText() = 0;

  virtual void EnableIME() = 0;

  virtual void DisableIME() = 0;

  virtual void CompleteComposition() = 0;

  virtual void CancelComposition() = 0;

  virtual const CompositionText& GetCompositionText() = 0;

  // Set the candidate window lefttop. Use this method to prepare typing.
  virtual void SetCandidateWindowPosition(const Point& point) = 0;

  // Triggered when user starts composition.
  virtual IEvent<std::nullptr_t>* CompositionStartEvent() = 0;

  // Triggered when user stops composition.
  virtual IEvent<std::nullptr_t>* CompositionEndEvent() = 0;

  // Triggered every time composition text changes.
  virtual IEvent<std::nullptr_t>* CompositionEvent() = 0;
};

struct IInputMethodManager : virtual INativeResource {
  virtual std::unique_ptr<IInputMethodContext> GetContext(
      INativeWindow* window) = 0;
};
}  // namespace cru::platform::native
