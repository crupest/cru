#include "cru/platform/gui/mock/InputMethod.h"

#include <format>
#include <sstream>
#include <string_view>
#include <utility>

namespace cru::platform::gui::mock {
MockInputMethodContext::MockInputMethodContext(
    bool manually_draw_composition_text)
    : manually_draw_composition_text_(manually_draw_composition_text) {}

bool MockInputMethodContext::ShouldManuallyDrawCompositionText() {
  return manually_draw_composition_text_;
}

void MockInputMethodContext::EnableIME() { ime_enabled_ = true; }

void MockInputMethodContext::DisableIME() {
  if (has_composition_) CancelComposition();
  ime_enabled_ = false;
}

void MockInputMethodContext::CompleteComposition() {
  if (!has_composition_) {
    SetNoActiveCompositionDiagnostic("CompleteComposition");
    return;
  }
  const auto text = composition_text_.text;
  CommitText(text);
}

void MockInputMethodContext::CancelComposition() {
  if (!has_composition_) {
    SetNoActiveCompositionDiagnostic("CancelComposition");
    return;
  }
  composition_text_ = {};
  FinishComposition();
}

CompositionText MockInputMethodContext::GetCompositionText() {
  return composition_text_;
}

void MockInputMethodContext::SetCandidateWindowPosition(const Point& point) {
  candidate_window_position_ = point;
}

void MockInputMethodContext::SetShouldManuallyDrawCompositionText(bool value) {
  manually_draw_composition_text_ = value;
}

std::string MockInputMethodContext::GetDiagnostic() const {
  std::ostringstream stream;
  stream << "ime_enabled=" << (ime_enabled_ ? "true" : "false")
         << ", has_composition=" << (has_composition_ ? "true" : "false")
         << ", composition_text_length=" << composition_text_.text.size();
  if (candidate_window_position_) {
    stream << ", candidate_window_position=(" << candidate_window_position_->x
           << ", " << candidate_window_position_->y << ")";
  } else {
    stream << ", candidate_window_position=(none)";
  }
  if (last_committed_text_) {
    stream << ", last_committed_text_length=" << last_committed_text_->size();
  } else {
    stream << ", last_committed_text=(none)";
  }
  if (!last_diagnostic_.empty()) {
    stream << ", last_diagnostic=\"" << last_diagnostic_ << "\"";
  }
  return stream.str();
}

void MockInputMethodContext::BeginComposition() {
  if (has_composition_) {
    last_diagnostic_ =
        "BeginComposition ignored because a composition is already active";
    return;
  }
  last_diagnostic_.clear();
  has_composition_ = true;
  CompositionStartEvent_.Raise(nullptr);
}

void MockInputMethodContext::UpdateComposition(
    CompositionText composition_text) {
  if (!has_composition_) {
    SetNoActiveCompositionDiagnostic("UpdateComposition");
    return;
  }
  RaiseCompositionUpdate(std::move(composition_text));
}

void MockInputMethodContext::UpdateComposition(std::string text,
                                               CompositionClauses clauses,
                                               TextRange selection) {
  UpdateComposition(
      CompositionText{std::move(text), std::move(clauses), selection});
}

void MockInputMethodContext::CommitText(std::string text) {
  if (has_composition_) {
    RaiseCompositionUpdate({});
    last_committed_text_ = text;
    TextEvent_.Raise(text);
    FinishComposition();
    return;
  }

  last_committed_text_ = text;
  TextEvent_.Raise(text);
}

void MockInputMethodContext::EndComposition() {
  if (!has_composition_) {
    SetNoActiveCompositionDiagnostic("EndComposition");
    return;
  }
  composition_text_ = {};
  FinishComposition();
}

void MockInputMethodContext::EmitText(std::string text) {
  CommitText(std::move(text));
}

void MockInputMethodContext::SetNoActiveCompositionDiagnostic(
    std::string_view operation) {
  last_diagnostic_ = std::format(
      "{} ignored because there is no active composition", operation);
}

void MockInputMethodContext::RaiseCompositionUpdate(
    CompositionText composition_text) {
  last_diagnostic_.clear();
  composition_text_ = std::move(composition_text);
  CompositionEvent_.Raise(nullptr);
}

void MockInputMethodContext::FinishComposition() {
  has_composition_ = false;
  CompositionEndEvent_.Raise(nullptr);
}
}  // namespace cru::platform::gui::mock
