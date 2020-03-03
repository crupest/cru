#include "cru/platform/heap_debug.hpp"
#include "cru/platform/native/window.hpp"
#include "cru/ui/controls/button.hpp"
#include "cru/ui/controls/flex_layout.hpp"
#include "cru/ui/controls/stack_layout.hpp"
#include "cru/ui/controls/text_block.hpp"
#include "cru/ui/window.hpp"
#include "cru/win/native/ui_application.hpp"

using cru::platform::native::win::WinUiApplication;
using cru::ui::Rect;
using cru::ui::Thickness;
using cru::ui::Window;
using cru::ui::controls::Button;
using cru::ui::controls::FlexLayout;
using cru::ui::controls::StackLayout;
using cru::ui::controls::TextBlock;

int main() {
#ifdef CRU_DEBUG
  cru::platform::SetupHeapDebug();
#endif

  std::unique_ptr<WinUiApplication> application =
      std::make_unique<WinUiApplication>();

  const auto window = Window::CreateOverlapped();

  const auto flex_layout = FlexLayout::Create();

  window->SetChild(flex_layout);

  const auto button = Button::Create();
  const auto text_block1 = TextBlock::Create();
  text_block1->SetText("Hello World!");
  button->SetChild(text_block1);
  flex_layout->AddChild(button, 0);

  const auto text_block2 = TextBlock::Create();
  text_block2->SetText("Hello World!");

  const auto text_block3 = TextBlock::Create();
  text_block3->SetText("Overlapped text");

  const auto stack_layout = StackLayout::Create();
  stack_layout->AddChild(text_block2, 0);
  stack_layout->AddChild(text_block3, 1);
  flex_layout->AddChild(stack_layout, 1);

  const auto text_block4 = TextBlock::Create();
  text_block4->SetText("Hello World!!!");
  flex_layout->AddChild(text_block4, 2);

  window->ResolveNativeWindow()->SetVisible(true);

  return application->Run();
}
