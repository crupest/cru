#include "cru/platform/Color.h"
#include "cru/platform/GraphicsBase.h"
#include "cru/platform/Matrix.h"
#include "cru/platform/bootstrap/Bootstrap.h"
#include "cru/platform/graphics/Brush.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/graphics/Font.h"
#include "cru/platform/graphics/Painter.h"
#include "cru/platform/graphics/TextLayout.h"
#include "cru/platform/gui/InputMethod.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/platform/gui/Window.h"

using namespace cru;
using namespace cru::platform;
using namespace cru::platform::graphics;
using namespace cru::platform::gui;

struct DemoBrushes {
  std::unique_ptr<ISolidColorBrush> black;
  std::unique_ptr<ISolidColorBrush> odd_clause;
  std::unique_ptr<ISolidColorBrush> even_clause;
  std::unique_ptr<ISolidColorBrush> target_clause;
};

class DemoWindow {
 public:
  DemoWindow(IUiApplication* application, DemoBrushes* brushes,
             std::shared_ptr<IFont> font);

 private:
  DemoBrushes* brushes_;
  std::unique_ptr<INativeWindow> window_;
  std::unique_ptr<ITextLayout> prompt_text_layout_;
  std::unique_ptr<ITextLayout> committed_text_layout_;
  String committed_text_;
  CompositionText composition_text_;
};

DemoWindow::DemoWindow(IUiApplication* application, DemoBrushes* brushes,
                       std::shared_ptr<IFont> font)
    : brushes_(brushes) {
  auto graphics_factory = application->GetGraphicsFactory();
  window_ = std::unique_ptr<INativeWindow>(application->CreateWindow());

  auto input_method_context = window_->GetInputMethodContext();

  prompt_text_layout_ =
      graphics_factory->CreateTextLayout(font,
                                         u"Ctrl+1: Enable IME\n"
                                         u"Ctrl+2: Disable IME\n"
                                         u"Ctrl+3: Complete composition.\n"
                                         u"Ctrl+4: Cancel composition.");
  committed_text_layout_ = graphics_factory->CreateTextLayout(font, u"");

  auto update_text_layout_width = [this](const Size& size) {
    prompt_text_layout_->SetMaxWidth(size.width);
    committed_text_layout_->SetMaxWidth(size.width);
  };

  update_text_layout_width(window_->GetClientSize());
  window_->ResizeEvent()->AddHandler(update_text_layout_width);

  window_->PaintEvent()->AddSpyOnlyHandler([this]() {
    auto painter = window_->BeginPaint();
    painter->Clear(colors::white);

    painter->DrawText(Point{}, prompt_text_layout_.get(),
                      brushes_->black.get());
    painter->PushState();
    painter->ConcatTransform(
        Matrix::Translation(0, prompt_text_layout_->GetTextBounds().height));

    for (int i = 0; i < static_cast<int>(composition_text_.clauses.size());
         i++) {
      const auto& clause = composition_text_.clauses[i];
      auto rects = committed_text_layout_->TextRangeRect(
          TextRange::FromTwoSides(clause.start - committed_text_.size(),
                                  clause.end - committed_text_.size()));
      const auto& b = clause.target ? brushes_->target_clause
                                    : (i % 2 ? brushes_->odd_clause
                                             : brushes_->even_clause);
      for (auto& rect : rects) {
        painter->FillRectangle(rect, b.get());
      }
    }

    painter->DrawText(Point{}, committed_text_layout_.get(),
                      brushes_->black.get());

    const auto cursor_pos = composition_text_.selection.position +
                            static_cast<int>(committed_text_.size());
    auto cursor_rect =
        committed_text_layout_->TextSinglePoint(cursor_pos, false);
    painter->FillRectangle(
        Rect{cursor_rect.left, cursor_rect.top, 3, cursor_rect.height},
        brushes_->black.get());

    painter->PopState();
    painter->EndDraw();
  });

  window_->KeyDownEvent()->AddHandler([this](const NativeKeyEventArgs& args) {
    auto input_method_context = window_->GetInputMethodContext();
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

  auto update_state = [this] {
    auto input_method_context = window_->GetInputMethodContext();
    committed_text_layout_->SetText(committed_text_ + composition_text_.text);
    auto y = prompt_text_layout_->GetTextBounds().height;
    const auto cursor_pos = composition_text_.selection.position +
                            static_cast<int>(committed_text_.size());
    auto cursor_rect =
        committed_text_layout_->TextSinglePoint(cursor_pos, false);
    input_method_context->SetCandidateWindowPosition(
        {cursor_rect.left, y + cursor_rect.GetBottom()});
    window_->RequestRepaint();
  };

  input_method_context->TextEvent()->AddHandler(
      [this, update_state](const StringView& c) {
        committed_text_ += c;
        update_state();
      });

  input_method_context->CompositionEvent()->AddHandler(
      [this, update_state](std::nullptr_t) {
        auto input_method_context = window_->GetInputMethodContext();
        composition_text_ = input_method_context->GetCompositionText();
        update_state();
      });

  input_method_context->CompositionEndEvent()->AddHandler(
      [this, update_state](std::nullptr_t) {
        composition_text_ = {};
        update_state();
      });

  window_->SetVisibility(WindowVisibilityType::Show);
}

int main() {
  IUiApplication* application = bootstrap::CreateUiApplication();
  application->SetQuitOnAllWindowClosed(true);

  auto graphics_factory = application->GetGraphicsFactory();

  DemoBrushes brushes{graphics_factory->CreateSolidColorBrush(colors::black),
                      graphics_factory->CreateSolidColorBrush(colors::yellow),
                      graphics_factory->CreateSolidColorBrush(colors::green),
                      graphics_factory->CreateSolidColorBrush(colors::blue)};

  std::shared_ptr<IFont> font = graphics_factory->CreateFont(String{}, 30);

  DemoWindow window1(application, &brushes, font);
  DemoWindow window2(application, &brushes, font);

  return application->Run();
}
