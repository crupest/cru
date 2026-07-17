#include "cru/platform/gui/mock/UiApplication.h"
#include "cru/platform/gui/mock/Window.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <string>
#include <vector>

using cru::platform::Point;
using cru::platform::TextRange;
using cru::platform::gui::CompositionText;
using cru::platform::gui::IInputMethodContext;
using cru::platform::gui::KeyCode;
using cru::platform::gui::KeyModifier;
using cru::platform::gui::KeyModifiers;
using cru::platform::gui::NativeKeyEventArgs;
using cru::platform::gui::mock::MockUiApplication;
using cru::platform::gui::mock::MockWindow;

TEST_CASE("Mock keyboard injection raises native key events in order",
          "[platform][gui][mock][input][MockKeyboardInjection]") {
  MockUiApplication app;
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());
  std::vector<std::string> events;
  std::vector<KeyCode> keys;
  std::vector<KeyModifier> modifiers;

  auto down_revoker =
      window->KeyDownEvent()->AddHandler([&](const NativeKeyEventArgs& args) {
        events.push_back("down");
        keys.push_back(args.key);
        modifiers.push_back(args.modifier);
      });
  auto up_revoker =
      window->KeyUpEvent()->AddHandler([&](const NativeKeyEventArgs& args) {
        events.push_back("up");
        keys.push_back(args.key);
        modifiers.push_back(args.modifier);
      });

  window->InjectKeyDown(KeyCode::A, KeyModifiers::Ctrl | KeyModifiers::Shift);
  window->InjectKeyUp(KeyCode::A, KeyModifiers::Alt);

  REQUIRE(events == std::vector<std::string>{"down", "up"});
  REQUIRE(keys == std::vector<KeyCode>{KeyCode::A, KeyCode::A});
  REQUIRE(modifiers ==
          std::vector<KeyModifier>{KeyModifiers::Ctrl | KeyModifiers::Shift,
                                   KeyModifiers::Alt});
}

TEST_CASE("Mock text input raises input method text event without composition",
          "[platform][gui][mock][input][MockTextInput]") {
  MockUiApplication app;
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());
  auto* input_method = window->GetMockInputMethodContext();
  IInputMethodContext* input_method_interface = window->GetInputMethodContext();
  std::vector<std::string> events;

  REQUIRE(input_method_interface == input_method);

  auto start_revoker =
      input_method_interface->CompositionStartEvent()->AddSpyOnlyHandler(
          [&] { events.push_back("start"); });
  auto composition_revoker =
      input_method_interface->CompositionEvent()->AddSpyOnlyHandler(
          [&] { events.push_back("composition"); });
  auto text_revoker = input_method_interface->TextEvent()->AddHandler(
      [&](const std::string& text) { events.push_back("text:" + text); });
  auto end_revoker =
      input_method_interface->CompositionEndEvent()->AddSpyOnlyHandler(
          [&] { events.push_back("end"); });

  window->InjectTextInput("plain");
  window->InjectCompositionCommit("");

  REQUIRE(events == std::vector<std::string>{"text:plain", "text:"});
  REQUIRE_FALSE(input_method->HasComposition());
  REQUIRE(input_method->GetCompositionText().text.empty());
  REQUIRE(input_method->GetLastCommittedText() == "");
}

TEST_CASE("Mock IME composition commit preserves native event ordering",
          "[platform][gui][mock][input][MockIme]") {
  MockUiApplication app;
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());
  auto* input_method = window->GetMockInputMethodContext();
  std::vector<std::string> events;

  auto start_revoker = input_method->CompositionStartEvent()->AddSpyOnlyHandler(
      [&] { events.push_back("start"); });
  auto composition_revoker =
      input_method->CompositionEvent()->AddSpyOnlyHandler([&] {
        events.push_back("composition:" +
                         input_method->GetCompositionText().text);
      });
  auto text_revoker = input_method->TextEvent()->AddHandler(
      [&](const std::string& text) { events.push_back("text:" + text); });
  auto end_revoker = input_method->CompositionEndEvent()->AddSpyOnlyHandler(
      [&] { events.push_back("end"); });

  window->InjectCompositionStart();
  window->InjectCompositionStart();
  window->InjectCompositionUpdate("draft", {{0, 5, true}}, TextRange{5});
  window->InjectCompositionUpdate(
      CompositionText{"draft2", {{0, 6, false}}, TextRange{1, 2}});
  window->InjectCompositionCommit("done");

  REQUIRE(events == std::vector<std::string>{
                        "start", "composition:draft", "composition:draft2",
                        "composition:", "text:done", "end"});
  REQUIRE_FALSE(input_method->HasComposition());
  REQUIRE(input_method->GetCompositionText().text.empty());
  REQUIRE(input_method->GetLastCommittedText() == "done");
}

TEST_CASE("Mock IME complete commits active composition and cancel drops it",
          "[platform][gui][mock][input][MockIme]") {
  MockUiApplication app;
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());
  auto* input_method = window->GetMockInputMethodContext();
  std::vector<std::string> events;

  auto start_revoker = input_method->CompositionStartEvent()->AddSpyOnlyHandler(
      [&] { events.push_back("start"); });
  auto composition_revoker =
      input_method->CompositionEvent()->AddSpyOnlyHandler([&] {
        events.push_back("composition:" +
                         input_method->GetCompositionText().text);
      });
  auto text_revoker = input_method->TextEvent()->AddHandler(
      [&](const std::string& text) { events.push_back("text:" + text); });
  auto end_revoker = input_method->CompositionEndEvent()->AddSpyOnlyHandler(
      [&] { events.push_back("end"); });

  window->InjectCompositionStart();
  window->InjectCompositionUpdate("convert");
  window->CompleteComposition();

  REQUIRE(events == std::vector<std::string>{"start", "composition:convert",
                                             "composition:", "text:convert",
                                             "end"});
  REQUIRE_FALSE(input_method->HasComposition());
  REQUIRE(input_method->GetLastCommittedText() == "convert");

  events.clear();
  window->InjectCompositionStart();
  window->InjectCompositionUpdate("drop");
  window->CancelComposition();

  REQUIRE(events ==
          std::vector<std::string>{"start", "composition:drop", "end"});
  REQUIRE_FALSE(input_method->HasComposition());
  REQUIRE(input_method->GetCompositionText().text.empty());
  REQUIRE(input_method->GetLastCommittedText() == "convert");
}

TEST_CASE("MockDiagnostics IME invalid no-composition sequences report state",
          "[platform][gui][mock][input][MockIme][MockDiagnostics]") {
  MockUiApplication app;
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());
  auto* input_method = window->GetMockInputMethodContext();
  std::vector<std::string> events;

  auto start_revoker = input_method->CompositionStartEvent()->AddSpyOnlyHandler(
      [&] { events.push_back("start"); });
  auto composition_revoker =
      input_method->CompositionEvent()->AddSpyOnlyHandler(
          [&] { events.push_back("composition"); });
  auto text_revoker = input_method->TextEvent()->AddHandler(
      [&](const std::string& text) { events.push_back("text:" + text); });
  auto end_revoker = input_method->CompositionEndEvent()->AddSpyOnlyHandler(
      [&] { events.push_back("end"); });

  window->InjectCompositionUpdate("ignored");
  REQUIRE(input_method->GetLastDiagnostic().find("UpdateComposition") !=
          std::string::npos);
  REQUIRE(input_method->GetDiagnostic().find("no active composition") !=
          std::string::npos);
  window->InjectCompositionEnd();
  REQUIRE(input_method->GetLastDiagnostic().find("EndComposition") !=
          std::string::npos);
  window->CompleteComposition();
  REQUIRE(input_method->GetLastDiagnostic().find("CompleteComposition") !=
          std::string::npos);
  window->CancelComposition();
  REQUIRE(input_method->GetLastDiagnostic().find("CancelComposition") !=
          std::string::npos);

  REQUIRE(events.empty());
  REQUIRE_FALSE(input_method->HasComposition());
  REQUIRE_FALSE(input_method->GetLastCommittedText().has_value());
  REQUIRE(window->GetLastInjectedEvent() == "CancelComposition");
  REQUIRE(window->GetDiagnostic().find(
              "last_injected_event=CancelComposition") != std::string::npos);
}

TEST_CASE("Mock IME candidate window position delegates to context state",
          "[platform][gui][mock][input][MockIme]") {
  MockUiApplication app;
  std::unique_ptr<MockWindow> window(app.CreateMockWindow());
  auto* input_method = window->GetMockInputMethodContext();

  REQUIRE_FALSE(input_method->GetCandidateWindowPosition().has_value());

  window->SetCandidateWindowPosition(Point{12.f, 34.f});

  REQUIRE(input_method->GetCandidateWindowPosition() == Point{12.f, 34.f});
}
