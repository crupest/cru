#pragma once

#include "Base.h"

#include <cru/base/Event.h>
#include <cru/platform/gui/InputMethod.h>

#include <optional>
#include <string>
#include <string_view>

namespace cru::platform::gui::mock {
class CRU_PLATFORM_GUI_MOCK_API MockInputMethodContext
    : public MockResource,
      public virtual IInputMethodContext {
 public:
  explicit MockInputMethodContext(bool manually_draw_composition_text = true);

  bool ShouldManuallyDrawCompositionText() override;

  void EnableIME() override;
  void DisableIME() override;

  // Complete commits the active composition text and ends composition. With no
  // active composition it is a no-op.
  void CompleteComposition() override;
  // Cancel clears the active composition and ends composition without
  // TextEvent. With no active composition it is a no-op.
  void CancelComposition() override;

  CompositionText GetCompositionText() override;
  void SetCandidateWindowPosition(const Point& point) override;

  CRU_DEFINE_CRU_PLATFORM_GUI_I_INPUT_METHOD_OVERRIDE_EVENTS()

  bool IsImeEnabled() const { return ime_enabled_; }
  bool HasComposition() const { return has_composition_; }
  std::optional<Point> GetCandidateWindowPosition() const {
    return candidate_window_position_;
  }
  std::optional<std::string> GetLastCommittedText() const {
    return last_committed_text_;
  }
  const std::string& GetLastDiagnostic() const { return last_diagnostic_; }
  std::string GetDiagnostic() const;

  void SetShouldManuallyDrawCompositionText(bool value);

  void BeginComposition();
  // Update and end are no-ops until BeginComposition starts a composition.
  void UpdateComposition(CompositionText composition_text);
  void UpdateComposition(std::string text, CompositionClauses clauses = {},
                         TextRange selection = {});
  // With an active composition, commit raises CompositionEvent, TextEvent, then
  // CompositionEndEvent. Without one, it is plain TextEvent input.
  void CommitText(std::string text);
  void EndComposition();
  void EmitText(std::string text);

 private:
  void SetNoActiveCompositionDiagnostic(std::string_view operation);
  void RaiseCompositionUpdate(CompositionText composition_text);
  void FinishComposition();

 private:
  bool manually_draw_composition_text_;
  bool ime_enabled_ = false;
  bool has_composition_ = false;
  CompositionText composition_text_;
  std::optional<Point> candidate_window_position_;
  std::optional<std::string> last_committed_text_;
  std::string last_diagnostic_;
};
}  // namespace cru::platform::gui::mock
