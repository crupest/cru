#pragma once
#include "../resource.hpp"
#include "base.hpp"

#include "cru/common/event.hpp"

#include <iostream>
#include <memory>
#include <vector>

namespace cru::platform::native {
struct CompositionClause {
  int start;
  int end;
  bool target;
};

using CompositionClauses = std::vector<CompositionClause>;

struct CompositionText {
  std::string text;
  CompositionClauses clauses;
  TextRange selection;
};

inline std::ostream& operator<<(std::ostream& stream,
                                const CompositionText& composition_text) {
  stream << "text: " << composition_text.text << "\n"
         << "clauses:\n";
  for (int i = 0; i < static_cast<int>(composition_text.clauses.size()); i++) {
    const auto& clause = composition_text.clauses[i];
    stream << "\t" << i << ". start:" << clause.start << " end:" << clause.end;
    if (clause.target) {
      stream << " target";
    }
    stream << "\n";
  }
  stream << "selection: position:" << composition_text.selection.position
         << " count:" << composition_text.selection.count;
  return stream;
}

struct IInputMethodContext : virtual INativeResource {
  // Return true if you should draw composition text manually. Return false if
  // system will take care of that for you.
  virtual bool ShouldManuallyDrawCompositionText() = 0;

  virtual void EnableIME() = 0;

  virtual void DisableIME() = 0;

  virtual void CompleteComposition() = 0;

  virtual void CancelComposition() = 0;

  virtual CompositionText GetCompositionText() = 0;

  // Set the candidate window lefttop. Use this method to prepare typing.
  virtual void SetCandidateWindowPosition(const Point& point) = 0;

  // Triggered when user starts composition.
  virtual IEvent<std::nullptr_t>* CompositionStartEvent() = 0;

  // Triggered when user stops composition.
  virtual IEvent<std::nullptr_t>* CompositionEndEvent() = 0;

  // Triggered every time composition text changes.
  virtual IEvent<std::nullptr_t>* CompositionEvent() = 0;

  virtual IEvent<std::string_view>* TextEvent() = 0;
};

struct IInputMethodManager : virtual INativeResource {
  virtual std::unique_ptr<IInputMethodContext> GetContext(
      INativeWindow* window) = 0;
};
}  // namespace cru::platform::native
