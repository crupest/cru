#pragma once
#include "cru/ui/components/Component.hpp"
#include "cru/ui/controls/Window.hpp"

namespace cru::theme_builder {
class MainWindow : public ui::components::Component {
 public:
  MainWindow();

  CRU_DELETE_COPY(MainWindow)
  CRU_DELETE_MOVE(MainWindow)

  ~MainWindow() override;

  ui::controls::Control* GetRootControl() override { return window_; }

  void Show();

 private:
  ui::controls::Window* window_;
};
}  // namespace cru::theme_builder
