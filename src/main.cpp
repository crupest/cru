#include "cru/platform/heap_debug.hpp"
#include "cru/platform/native/native_window.hpp"
#include "cru/platform/native/ui_application.hpp"
#include "cru/ui/controls/button.hpp"
#include "cru/ui/controls/flex_layout.hpp"
#include "cru/ui/controls/text_block.hpp"
#include "cru/ui/window.hpp"

using cru::platform::native::UiApplication;
using cru::ui::Rect;
using cru::ui::Thickness;
using cru::ui::Window;
using cru::ui::controls::Button;
using cru::ui::controls::FlexLayout;
using cru::ui::controls::TextBlock;


int main() {
#ifdef CRU_DEBUG
  cru::platform::SetupHeapDebug();
#endif

  std::unique_ptr<UiApplication> application(UiApplication::CreateInstance());

  const auto window = Window::CreateOverlapped();

  const auto flex_layout = FlexLayout::Create();

  window->SetChild(flex_layout);

  const auto button = Button::Create();
  const auto text_block1 = TextBlock::Create();
  text_block1->SetText(L"Hello World!");
  button->SetChild(text_block1);
  flex_layout->AddChild(button, 0);

  const auto text_block2 = TextBlock::Create();
  text_block2->SetText(L"Hello World!");
  flex_layout->AddChild(text_block2, 1);

  window->GetNativeWindow()->SetVisible(true);

  return application->Run();
}
