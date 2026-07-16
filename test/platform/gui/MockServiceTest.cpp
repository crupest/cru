#include "cru/platform/gui/mock/Clipboard.h"
#include "cru/platform/gui/mock/Cursor.h"
#include "cru/platform/gui/mock/InputMethod.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <string>
#include <vector>

using cru::platform::Point;
using cru::platform::TextRange;
using cru::platform::gui::CompositionText;
using cru::platform::gui::SystemCursorType;
using cru::platform::gui::mock::MockClipboard;
using cru::platform::gui::mock::MockCursor;
using cru::platform::gui::mock::MockCursorManager;
using cru::platform::gui::mock::MockInputMethodContext;
using cru::platform::gui::mock::MockResource;

TEST_CASE("Mock GUI service platform ids are stable",
          "[platform][gui][mock][resource]") {
  const MockCursor cursor(SystemCursorType::Arrow);
  const MockCursorManager cursor_manager;
  const MockClipboard clipboard;
  const MockInputMethodContext input_method;

  REQUIRE(cursor.GetPlatformId() == MockResource::kPlatformId);
  REQUIRE(cursor_manager.GetPlatformId() == MockResource::kPlatformId);
  REQUIRE(clipboard.GetPlatformId() == MockResource::kPlatformId);
  REQUIRE(input_method.GetPlatformId() == MockResource::kPlatformId);
}

TEST_CASE("Mock cursor manager returns stable system cursors",
          "[platform][gui][mock][cursor]") {
  MockCursorManager manager;

  const auto arrow = manager.GetSystemCursor(SystemCursorType::Arrow);
  const auto arrow_again = manager.GetSystemCursor(SystemCursorType::Arrow);
  const auto hand = manager.GetSystemCursor(SystemCursorType::Hand);
  const auto ibeam = manager.GetSystemCursor(SystemCursorType::IBeam);

  REQUIRE(arrow == arrow_again);
  REQUIRE(arrow != hand);
  REQUIRE(hand != ibeam);
  REQUIRE(std::dynamic_pointer_cast<MockCursor>(arrow)->GetType() ==
          SystemCursorType::Arrow);
  REQUIRE(std::dynamic_pointer_cast<MockCursor>(hand)->GetType() ==
          SystemCursorType::Hand);
  REQUIRE(std::dynamic_pointer_cast<MockCursor>(ibeam)->GetType() ==
          SystemCursorType::IBeam);
}

TEST_CASE("Mock clipboard round-trips text deterministically",
          "[platform][gui][mock][clipboard]") {
  MockClipboard clipboard;

  REQUIRE(clipboard.GetText().empty());

  clipboard.SetText("hello");
  REQUIRE(clipboard.GetText() == "hello");

  clipboard.SetText("");
  REQUIRE(clipboard.GetText().empty());

  clipboard.SetText("changed text");
  clipboard.Clear();
  REQUIRE(clipboard.GetText().empty());
}

TEST_CASE("Mock input method tracks state and candidate position",
          "[platform][gui][mock][ime]") {
  MockInputMethodContext input_method;

  REQUIRE(input_method.ShouldManuallyDrawCompositionText());
  REQUIRE_FALSE(input_method.IsImeEnabled());
  REQUIRE_FALSE(input_method.HasComposition());
  REQUIRE_FALSE(input_method.GetCandidateWindowPosition().has_value());

  input_method.SetShouldManuallyDrawCompositionText(false);
  input_method.EnableIME();
  input_method.SetCandidateWindowPosition(Point{12.f, 34.f});

  REQUIRE_FALSE(input_method.ShouldManuallyDrawCompositionText());
  REQUIRE(input_method.IsImeEnabled());
  REQUIRE(input_method.GetCandidateWindowPosition() == Point{12.f, 34.f});

  input_method.BeginComposition();
  input_method.UpdateComposition(
      CompositionText{"draft", {{0, 5, true}}, TextRange{2}});

  REQUIRE(input_method.HasComposition());
  REQUIRE(input_method.GetCompositionText().text == "draft");
  REQUIRE(input_method.GetCompositionText().selection == TextRange{2});
  REQUIRE(input_method.GetCompositionText().clauses.size() == 1);
  REQUIRE(input_method.GetCompositionText().clauses[0].start == 0);
  REQUIRE(input_method.GetCompositionText().clauses[0].end == 5);
  REQUIRE(input_method.GetCompositionText().clauses[0].target);

  input_method.DisableIME();

  REQUIRE_FALSE(input_method.IsImeEnabled());
  REQUIRE_FALSE(input_method.HasComposition());
  REQUIRE(input_method.GetCompositionText().text.empty());
}

TEST_CASE("Mock input method preserves composition commit event order",
          "[platform][gui][mock][ime]") {
  MockInputMethodContext input_method;
  std::vector<std::string> events;

  auto start_revoker = input_method.CompositionStartEvent()->AddSpyOnlyHandler(
      [&events] { events.push_back("start"); });
  auto composition_revoker =
      input_method.CompositionEvent()->AddSpyOnlyHandler([&] {
        events.push_back("composition:" +
                         input_method.GetCompositionText().text);
      });
  auto text_revoker = input_method.TextEvent()->AddHandler(
      [&events](const std::string& text) { events.push_back("text:" + text); });
  auto end_revoker = input_method.CompositionEndEvent()->AddSpyOnlyHandler(
      [&events] { events.push_back("end"); });

  input_method.BeginComposition();
  input_method.UpdateComposition("preedit");
  input_method.CommitText("done");

  REQUIRE(events == std::vector<std::string>{"start", "composition:preedit",
                                             "composition:", "text:done",
                                             "end"});
  REQUIRE_FALSE(input_method.HasComposition());
  REQUIRE(input_method.GetLastCommittedText() == "done");
}

TEST_CASE("Mock input method no-composition operations are deterministic",
          "[platform][gui][mock][ime]") {
  MockInputMethodContext input_method;
  std::vector<std::string> events;

  auto start_revoker = input_method.CompositionStartEvent()->AddSpyOnlyHandler(
      [&events] { events.push_back("start"); });
  auto composition_revoker = input_method.CompositionEvent()->AddSpyOnlyHandler(
      [&events] { events.push_back("composition"); });
  auto text_revoker = input_method.TextEvent()->AddHandler(
      [&events](const std::string& text) { events.push_back("text:" + text); });
  auto end_revoker = input_method.CompositionEndEvent()->AddSpyOnlyHandler(
      [&events] { events.push_back("end"); });

  input_method.CancelComposition();
  input_method.EndComposition();
  input_method.CompleteComposition();
  input_method.UpdateComposition("ignored");
  input_method.EmitText("plain");

  REQUIRE(events == std::vector<std::string>{"text:plain"});
  REQUIRE_FALSE(input_method.HasComposition());
  REQUIRE(input_method.GetLastCommittedText() == "plain");
}
