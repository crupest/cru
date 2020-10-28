#pragma once
#include "ContentControl.hpp"

namespace cru::ui {
class Window final : public ContentControl {
  friend WindowHost;

 public:
  static constexpr std::u16string_view control_type = u"Window";

 public:
  static Window* CreateOverlapped();

 private:
  struct tag_overlapped_constructor {};

  explicit Window(tag_overlapped_constructor);

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
  void OnChildChanged(Control* old_child, Control* new_child) override;

 private:
  std::unique_ptr<WindowHost> managed_ui_host_;

  // WindowHost is responsible to take care of lifetime of this.
  render::WindowRenderObject* render_object_;
};
}  // namespace cru::ui
