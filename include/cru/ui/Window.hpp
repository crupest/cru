#pragma once
#include "LayoutControl.hpp"

namespace cru::ui {
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

 protected:
  void OnAddChild(Control* child, Index position) override;
  void OnRemoveChild(Control* child, Index position) override;

 private:
  std::unique_ptr<WindowHost> window_host_;

  std::unique_ptr<render::StackLayoutRenderObject> render_object_;
};
}  // namespace cru::ui
