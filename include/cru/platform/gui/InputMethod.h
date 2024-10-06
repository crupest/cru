#pragma once
#include "Base.h"

#include "cru/base/Event.h"

#include <memory>
#include <vector>

namespace cru::platform::gui {
struct CompositionClause {
  int start;
  int end;
  bool target;
};

using CompositionClauses = std::vector<CompositionClause>;

struct CompositionText {
  String text;
  CompositionClauses clauses;
  TextRange selection;
};

/**
 * \remarks I think it's time to standatdize this. The most important thing is
 * the events.
 *
 * The events hould be triggered in this way.
 * 1. Any time the IME begins to work, CompositionStartEvent is fired. Only
 * once. Not triggerred again until CompositionEndEvent is fired.
 * 2. Any time composition state changed, maybe user typed more characters, or
 * user commit part of composition, CompositionEvent is fired.
 * 3. TextEvent is fired when user commit part or whole of the composition. And
 * you can use the args to get what characters are committed. So it is where you
 * get the real text user want to give you.
 * 4. Whenever a commit happens, TextEvent first, followed by CompositionEvent.
 * Each for once. So use the TextEvent to get real input and use
 * CompositionEvent to update UI.
 * 5. When composition stops, a final CompositionEndEvent is fired. Also only
 * once.
 */
struct IInputMethodContext : virtual IPlatformResource {
  // Return true if you should draw composition text manually. Return false if
  // system will take care of that for you.
  virtual bool ShouldManuallyDrawCompositionText() = 0;

  virtual void EnableIME() = 0;

  virtual void DisableIME() = 0;

  virtual void CompleteComposition() = 0;

  virtual void CancelComposition() = 0;

  virtual CompositionText GetCompositionText() = 0;

  // Set the candidate window left-top. Relative to window left-top. Use this
  // method to prepare typing.
  virtual void SetCandidateWindowPosition(const Point& point) = 0;

  virtual IEvent<std::nullptr_t>* CompositionStartEvent() = 0;
  virtual IEvent<std::nullptr_t>* CompositionEndEvent() = 0;
  virtual IEvent<std::nullptr_t>* CompositionEvent() = 0;
  virtual IEvent<StringView>* TextEvent() = 0;
};
}  // namespace cru::platform::gui
