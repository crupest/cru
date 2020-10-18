#include "cru/platform/HeapDebug.hpp"
#include "cru/platform/native/UiApplication.hpp"
#include "cru/platform/native/Window.hpp"
#include "cru/ui/Base.hpp"
#include "cru/ui/UiHost.hpp"
#include "cru/ui/Window.hpp"
#include "cru/ui/controls/Button.hpp"
#include "cru/ui/controls/FlexLayout.hpp"
#include "cru/ui/controls/StackLayout.hpp"
#include "cru/ui/controls/TextBlock.hpp"
#include "cru/ui/controls/TextBox.hpp"

using cru::platform::native::CreateUiApplication;
using cru::ui::Window;
using cru::ui::controls::Button;
using cru::ui::controls::FlexLayout;
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
  flex_layout->SetFlexDirection(cru::ui::FlexDirection::Vertical);

  const auto text_block = TextBlock::Create();
  text_block->SetText(u"Hello World from CruUI!");
  flex_layout->AddChild(text_block, 0);

  const auto button_text_block = TextBlock::Create();
  button_text_block->SetText(u"OK");
  const auto button = Button::Create();
  button->SetChild(button_text_block);
  flex_layout->AddChild(button, 1);

  const auto text_box = TextBox::Create();
  flex_layout->AddChild(text_box, 2);

  window->GetUiHost()->GetNativeWindowResolver()->Resolve()->SetVisible(true);

  return application->Run();
}
