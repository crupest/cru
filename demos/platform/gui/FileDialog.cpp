#include <cru/platform/bootstrap/Bootstrap.h>
#include <cru/platform/gui/SaveOpenDialogOptions.h>
#include <cru/platform/gui/Window.h>
#include <cru/ui/components/TextButton.h>
#include <cru/ui/controls/FlexLayout.h>
#include <cru/ui/controls/Window.h>

int main() {
  using namespace cru::platform::gui;
  using namespace cru::ui::controls;
  using namespace cru::ui::components;
  std::unique_ptr<IUiApplication> application(
      cru::platform::bootstrap::CreateUiApplication());

  Window window;
  FlexLayout layout;
  window.AddChild(&layout);

  TextButton open_button("Open");
  TextButton open_dir_button("Open Dir");
  TextButton save_button("Save");
  layout.SetFlexDirection(FlexDirection::Vertical);
  layout.AddChild(open_button.GetRootControl());
  layout.AddChild(open_dir_button.GetRootControl());
  layout.AddChild(save_button.GetRootControl());

  open_button.ClickEvent()->AddSpyOnlyHandler([&application] {
    auto options = OpenDialogOptionsBuilder()
                       .SetTitle("Open Files")
                       .SetPrompt("Files to open")
                       .AddAllowedFileType("txt")
                       .SetAllowAllFileTypes(true)
                       .SetCanChooseFiles(true)
                       .SetCanChooseDirectories(false)
                       .SetAllowMultipleSelection(true)
                       .Build();

    application->ShowOpenDialog(options);
  });

  open_dir_button.ClickEvent()->AddSpyOnlyHandler([&application] {
    auto options = OpenDialogOptionsBuilder()
                       .SetTitle("Open Dir")
                       .SetPrompt("Dir to open")
                       .SetCanChooseFiles(false)
                       .SetCanChooseDirectories(true)
                       .SetAllowMultipleSelection(true)
                       .Build();
    application->ShowOpenDialog(options);
  });

  save_button.ClickEvent()->AddSpyOnlyHandler([&application] {
    auto options = SaveDialogOptionsBuilder()
                       .SetTitle("Save File")
                       .SetPrompt("File to save")
                       .AddAllowedFileType("txt")
                       .SetAllowAllFileTypes(true)
                       .Build();
    application->ShowSaveDialog(options);
  });

  window.GetNativeWindow()->SetVisibility(WindowVisibilityType::Show);

  return application->Run();
}
