#pragma once
#include "LayoutControl.hpp"

#include <memory>

namespace cru::ui::controls {
class Popup : public LayoutControl {
 public:
  explicit Popup(Control* attached_control = nullptr);

  CRU_DELETE_COPY(Popup)
  CRU_DELETE_MOVE(Popup)

  ~Popup() override;

 private:
  std::unique_ptr<host::WindowHost> window_host_;

  std::unique_ptr<render::StackLayoutRenderObject> render_object_;

  Control* attached_control_;
};
}  // namespace cru::ui::controls
