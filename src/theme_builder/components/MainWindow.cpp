#include "MainWindow.hpp"

namespace cru::theme_builder {
using namespace cru::ui;
using namespace cru::ui::controls;
using namespace cru::platform::gui;

MainWindow::MainWindow() {
  window_ = Window::Create();
  window_->GetNativeWindow()->SetTitle(u"CruUI Theme Builder");
}

MainWindow::~MainWindow() { delete window_; }

void MainWindow::Show() {
  window_->GetNativeWindow()->SetVisibility(WindowVisibilityType::Show);
  window_->GetNativeWindow()->SetToForeground();
}
}  // namespace cru::theme_builder
