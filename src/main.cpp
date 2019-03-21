#include "pre.hpp"

#include "application.hpp"
#include "ui/controls/flex_layout.hpp"
#include "ui/controls/text_block.hpp"
#include "ui/window.hpp"

using cru::Application;
using cru::String;
using cru::StringView;
using cru::ui::Rect;
using cru::ui::Thickness;
using cru::ui::Window;
using cru::ui::controls::FlexLayout;
using cru::ui::controls::TextBlock;

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                      LPWSTR lpCmdLine, int nCmdShow) {
#ifdef CRU_DEBUG
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  Application application(hInstance);

  const auto window = Window::CreateOverlapped();

  const auto flex_layout = FlexLayout::Create();

  window->SetChild(flex_layout);

  const auto text_block1 = TextBlock::Create();
  text_block1->SetText(L"Hello World!");
  flex_layout->AddChild(text_block1, 0);

  const auto text_block2 = TextBlock::Create();
  text_block2->SetText(L"Hello World!");
  flex_layout->AddChild(text_block2, 1);


  window->Show();

  return application.Run();
}
