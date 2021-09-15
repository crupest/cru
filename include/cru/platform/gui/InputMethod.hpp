#pragma once
#include "Base.hpp"

#include "cru/common/Event.hpp"

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

struct IInputMethodContext : virtual IPlatformResource {
  // Return true if you should draw composition text manually. Return false if
  // system will take care of that for you.
  virtual bool ShouldManuallyDrawCompositionText() = 0;

  virtual void EnableIME() = 0;

  virtual void DisableIME() = 0;

  virtual void CompleteComposition() = 0;

  virtual void CancelComposition() = 0;

  virtual CompositionText GetCompositionText() = 0;

  // Set the candidate window lefttop. Relative to window lefttop. Use this
  // method to prepare typing.
  virtual void SetCandidateWindowPosition(const Point& point) = 0;

  // Triggered when user starts composition.
  virtual IEvent<std::nullptr_t>* CompositionStartEvent() = 0;

  // Triggered when user stops composition.
  virtual IEvent<std::nullptr_t>* CompositionEndEvent() = 0;

  // Triggered every time composition text changes.
  virtual IEvent<std::nullptr_t>* CompositionEvent() = 0;

  virtual IEvent<StringView>* TextEvent() = 0;
};
}  // namespace cru::platform::gui

// template <>
// struct fmt::formatter<cru::platform::gui::CompositionText, char16_t>
//     : fmt::formatter<std::u16string_view, char16_t> {
//   auto parse(fmt::basic_format_parse_context<char16_t>& ctx) {
//     return fmt::formatter<std::u16string_view, char16_t>::parse(ctx);
//   }

//   template <typename FormatContext>
//   auto format(const cru::platform::gui::CompositionText& ct,
//               FormatContext& ctx) {
//     auto output = ctx.out();
//     output = format_to(output, u"text: {}\n", ct.text);
//     output = format_to(output, u"clauses:\n");
//     for (gsl::index i = 0; i < static_cast<gsl::index>(ct.clauses.size());
//          i++) {
//       const auto& clause = ct.clauses[i];
//       output =
//           format_to(output, u"\t{}. start: {} end: {}{}\n", i, clause.start,
//                     clause.end, clause.target ? u" target" : u"");
//     }
//     output = format_to(output, u"selection: position: {} count: {}",
//                        ct.selection.position, ct.selection.count);
//     return output;
//   }
// };
