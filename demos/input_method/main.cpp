#include "cru/platform/graph/factory.hpp"
#include "cru/platform/graph/font.hpp"
#include "cru/platform/graph/painter.hpp"
#include "cru/platform/native/input_method.hpp"
#include "cru/platform/native/ui_application.hpp"
#include "cru/platform/native/window.hpp"

int main() {
  using namespace cru::platform;
  using namespace cru::platform::graph;
  using namespace cru::platform::native;

  auto application = CreateUiApplication();

  auto graph_factory = application->GetGraphFactory();

  auto window_resolver = application->CreateWindow(nullptr);

  auto window = window_resolver->Resolve();

  auto input_method_context =
      application->GetInputMethodManager()->GetContext(window);

  auto brush = graph_factory->CreateSolidColorBrush();
  brush->SetColor(colors::black);

  auto odd_clause_brush = graph_factory->CreateSolidColorBrush();
  odd_clause_brush->SetColor(colors::yellow);
  auto even_clause_brush = graph_factory->CreateSolidColorBrush();
  even_clause_brush->SetColor(colors::green);
  auto target_clause_brush = graph_factory->CreateSolidColorBrush();
  target_clause_brush->SetColor(colors::blue);

  std::shared_ptr<IFont> font = graph_factory->CreateFont("等线", 30);

  float window_width = 10000;

  auto prompt_text_layout =
      graph_factory->CreateTextLayout(font,
                                      "Alt+F1: Enable IME\n"
                                      "Alt+F2: Disable IME\n"
                                      "Alt+F3: Complete composition.\n"
                                      "Alt+F4: Cancel composition.");

  std::optional<CompositionText> optional_composition_text;
  std::string committed_text;

  window->ResizeEvent()->AddHandler(
      [&prompt_text_layout, &window_width](const Size& size) {
        prompt_text_layout->SetMaxWidth(size.width);
        window_width = size.width;
      });

  window->PaintEvent()->AddHandler([&](auto) {
    auto painter = window->BeginPaint();
    painter->Clear(colors::white);

    painter->DrawText(Point{}, prompt_text_layout.get(), brush.get());

    const auto anchor_y = prompt_text_layout->GetTextBounds().height;

    auto text_layout = graph_factory->CreateTextLayout(
        font,
        committed_text +
            (optional_composition_text ? optional_composition_text->text : ""));

    if (optional_composition_text) {
      const auto& composition_text = *optional_composition_text;

      for (int i = 0; i < composition_text.clauses.size(); i++) {
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

    if (optional_composition_text) {
      const auto& composition_text = *optional_composition_text;

      const auto cursor_pos = composition_text.selection.position +
                              gsl::narrow_cast<int>(committed_text.size());

      const auto cursor_lefttop =
          text_layout->TextSingleRect(cursor_pos, false);

      painter->FillRectangle(Rect{cursor_lefttop.x, cursor_lefttop.y + anchor_y,
                                  3, font->GetFontSize()},
                             brush.get());
    }
  });

  window->KeyDownEvent()->AddHandler(
      [&input_method_context](const NativeKeyEventArgs& args) {
        if (args.modifier & key_modifiers::alt) {
          switch (args.key) {
            case KeyCode::F1:
              input_method_context->EnableIME();
              break;
            case KeyCode::F2:
              input_method_context->DisableIME();
              break;
            case KeyCode::F3:
              input_method_context->CompleteComposition();
              break;
            case KeyCode::F4:
              input_method_context->CancelComposition();
              break;
            default:
              break;
          }
        }
      });

  input_method_context->TextEvent()->AddHandler(
      [window, &committed_text](const std::string_view& c) {
        committed_text += c;
        window->RequestRepaint();
      });

  input_method_context->CompositionEvent()->AddHandler(
      [window, &input_method_context, &optional_composition_text](auto) {
        optional_composition_text = input_method_context->GetCompositionText();
        window->RequestRepaint();
      });

  input_method_context->CompositionEndEvent()->AddHandler(
      [window, &input_method_context, &optional_composition_text](auto) {
        optional_composition_text = std::nullopt;
        window->RequestRepaint();
      });

  window->SetVisible(true);

  return application->Run();
}
