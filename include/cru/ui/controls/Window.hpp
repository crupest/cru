#pragma once
#include "LayoutControl.hpp"

namespace cru::ui::controls {
class Window final : public LayoutControl {
 public:
  static constexpr std::u16string_view control_type = u"Window";

 public:
  static Window* CreateOverlapped();

 private:
  Window();

 public:
  Window(const Window& other) = delete;
  Window(Window&& other) = delete;
  Window& operator=(const Window& other) = delete;
  Window& operator=(Window&& other) = delete;
  ~Window() override;

 public:
  std::u16string_view GetControlType() const final;

  render::RenderObject* GetRenderObject() const override;

  // If create is false and native window is not create, it will not be created
  // and shown.
  void Show(bool create = true);

 private:
  std::unique_ptr<host::WindowHost> window_host_;

  std::unique_ptr<render::StackLayoutRenderObject> render_object_;
};
}  // namespace cru::ui::controls
