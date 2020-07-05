#include "cru/platform/HeapDebug.hpp"
#include "cru/platform/native/UiApplication.hpp"
#include "cru/platform/native/Window.hpp"
#include "cru/ui/UiHost.hpp"
#include "cru/ui/Window.hpp"
#include "cru/ui/controls/Button.hpp"
#include "cru/ui/controls/FlexLayout.hpp"
#include "cru/ui/controls/StackLayout.hpp"
#include "cru/ui/controls/TextBlock.hpp"
#include "cru/ui/controls/TextBox.hpp"

using cru::platform::native::CreateUiApplication;
using cru::ui::Rect;
using cru::ui::Thickness;
using cru::ui::Window;
using cru::ui::controls::Button;
using cru::ui::controls::FlexLayout;
using cru::ui::controls::StackLayout;
using cru::ui::controls::TextBlock;
using cru::ui::controls::TextBox;

int main() {
#ifdef CRU_DEBUG
  cru::platform::SetupHeapDebug();
#endif

  auto application = CreateUiApplication();

  const auto window = Window::CreateOverlapped();

  const auto flex_layout = FlexLayout::Create();

  window->SetChild(flex_layout);

  const auto button = Button::Create();
  const auto text_block1 = TextBlock::Create();
  text_block1->SetText(u"Hello World!");
  button->SetChild(text_block1);
  flex_layout->AddChild(button, 0);

  const auto text_block2 = TextBlock::Create();
  text_block2->SetText(u"Hello World!");

  const auto text_block3 = TextBlock::Create();
  text_block3->SetText(u"Overlapped text");

  const auto stack_layout = StackLayout::Create();
  stack_layout->AddChild(text_block2, 0);
  stack_layout->AddChild(text_block3, 1);
  flex_layout->AddChild(stack_layout, 1);

  const auto text_block4 = TextBlock::Create();
  text_block4->SetText(u"Hello World!!!");
  flex_layout->AddChild(text_block4, 2);

  window->GetUiHost()->GetNativeWindowResolver()->Resolve()->SetVisible(true);

  return application->Run();
}
