#include "cru/platform/native/native_window.hpp"
#include "cru/platform/native/ui_applicaition.hpp"
#include "cru/ui/controls/button.hpp"
#include "cru/ui/controls/flex_layout.hpp"
#include "cru/ui/controls/text_block.hpp"
#include "cru/ui/window.hpp"

using cru::platform::native::IUiApplication;
using cru::ui::Rect;
using cru::ui::Thickness;
using cru::ui::Window;
using cru::ui::controls::Button;
using cru::ui::controls::FlexLayout;
using cru::ui::controls::TextBlock;
/*
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                      LPWSTR lpCmdLine, int nCmdShow) {
#ifdef CRU_DEBUG
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
*/

int main() {
  std::unique_ptr<IUiApplication> application(IUiApplication::CreateInstance());

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
