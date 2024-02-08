#include "cru/platform/Color.h"
#include "cru/platform/GraphicsBase.h"
#include "cru/platform/bootstrap/Bootstrap.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/graphics/Font.h"
#include "cru/platform/graphics/Painter.h"
#include "cru/platform/gui/InputMethod.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/platform/gui/Window.h"

#include <optional>

using namespace cru;
using namespace cru::platform;
using namespace cru::platform::graphics;
using namespace cru::platform::gui;

struct InputMethodState {
  CompositionText composition_text;
  Rect cursor_rect;
  TextRange colored_text_range;
};

int main() {
  IUiApplication* application = bootstrap::CreateUiApplication();

  auto graphics_factory = application->GetGraphicsFactory();

  auto window = application->CreateWindow();

  auto input_method_context = window->GetInputMethodContext();

  auto brush = graphics_factory->CreateSolidColorBrush();
  brush->SetColor(colors::black);

  auto odd_clause_brush = graphics_factory->CreateSolidColorBrush();
  odd_clause_brush->SetColor(colors::yellow);
  auto even_clause_brush = graphics_factory->CreateSolidColorBrush();
  even_clause_brush->SetColor(colors::green);
  auto target_clause_brush = graphics_factory->CreateSolidColorBrush();
  target_clause_brush->SetColor(colors::blue);

  std::shared_ptr<IFont> font = graphics_factory->CreateFont(String{}, 30);

  auto prompt_text_layout =
      graphics_factory->CreateTextLayout(font,
                                         u"Ctrl+1: Enable IME\n"
                                         u"Ctrl+2: Disable IME\n"
                                         u"Ctrl+3: Complete composition.\n"
                                         u"Ctrl+4: Cancel composition.");
  float anchor_y;

  String committed_text;
  auto text_layout = graphics_factory->CreateTextLayout(font, u"");
  std::optional<InputMethodState> state;

  auto update_text_layout_width = [&prompt_text_layout, &anchor_y,
                                   &text_layout](float width) {
    prompt_text_layout->SetMaxWidth(width);
    text_layout->SetMaxWidth(width);
    anchor_y = prompt_text_layout->GetTextBounds().height;
  };

  update_text_layout_width(window->GetClientSize().width);

  window->ResizeEvent()->AddHandler(
      [&update_text_layout_width](const Size& size) {
        update_text_layout_width(size.width);
      });

  window->PaintEvent()->AddHandler([&](auto) {
    auto painter = window->BeginPaint();
    painter->Clear(colors::white);

    painter->DrawText(Point{}, prompt_text_layout.get(), brush.get());

    if (state) {
      const auto& composition_text = state->composition_text;

      for (int i = 0; i < static_cast<int>(composition_text.clauses.size());
           i++) {
        const auto& clause = composition_text.clauses[i];
        auto rects = text_layout->TextRangeRect(TextRange::FromTwoSides(
            clause.start, clause.end, committed_text.size()));
        const auto& b = clause.target
                            ? target_clause_brush
                            : (i % 2 ? odd_clause_brush : even_clause_brush);
        for (auto& rect : rects) {
          rect.top += anchor_y;
          painter->FillRectangle(rect, b.get());
        }
      }
    }

    painter->DrawText(Point{0, anchor_y}, text_layout.get(), brush.get());

    if (state) {
      const auto& composition_text = state->composition_text;
      const auto& cursor_rect = state->cursor_rect;

      painter->FillRectangle(Rect{cursor_rect.left, cursor_rect.top + anchor_y,
                                  3, cursor_rect.height},
                             brush.get());
    }

    painter->EndDraw();
  });

  window->KeyDownEvent()->AddHandler(
      [&input_method_context](const NativeKeyEventArgs& args) {
        if (args.modifier & KeyModifiers::ctrl) {
          switch (args.key) {
            case KeyCode::N1:
              input_method_context->EnableIME();
              break;
            case KeyCode::N2:
              input_method_context->DisableIME();
              break;
            case KeyCode::N3:
              input_method_context->CompleteComposition();
              break;
            case KeyCode::N4:
              input_method_context->CancelComposition();
              break;
            default:
              break;
          }
        }
      });

  input_method_context->TextEvent()->AddHandler(
      [window, &committed_text](const StringView& c) {
        committed_text += c;
        window->RequestRepaint();
      });

  input_method_context->CompositionEvent()->AddHandler([window,
                                                        &input_method_context,
                                                        &committed_text,
                                                        &anchor_y, &state,
                                                        &text_layout](auto) {
    const auto composition_text = input_method_context->GetCompositionText();
    state.emplace();
    state->composition_text = input_method_context->GetCompositionText();

    text_layout->SetText(committed_text + composition_text.text);

    const auto cursor_pos = composition_text.selection.position +
                            static_cast<int>(committed_text.size());
    state->cursor_rect = text_layout->TextSinglePoint(cursor_pos, false);

    input_method_context->SetCandidateWindowPosition(
        {state->cursor_rect.left, anchor_y + state->cursor_rect.GetBottom()});

    window->RequestRepaint();
  });

  input_method_context->CompositionEndEvent()->AddHandler(
      [window, &state](auto) {
        state = std::nullopt;
        window->RequestRepaint();
      });

  window->SetVisibility(WindowVisibilityType::Show);

  return application->Run();
}
