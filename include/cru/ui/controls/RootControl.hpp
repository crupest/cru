#pragma once
#include "LayoutControl.hpp"

#include "cru/common/Base.hpp"
#include "cru/platform/gui/Base.hpp"
#include "cru/ui/Base.hpp"

namespace cru::ui::controls {
class RootControl : public LayoutControl {
 protected:
  explicit RootControl(Control* attached_control);

 public:
  CRU_DELETE_COPY(RootControl)
  CRU_DELETE_MOVE(RootControl)
  ~RootControl() override;

 public:
  render::RenderObject* GetRenderObject() const override;

  void EnsureWindowCreated();

  // If create is false and native window is not create, it will not be created
  // and shown.
  void Show(bool create = true);

  // If native window does not exist, nothing will be done. It will not save it
  // and use it when creating window. So call this after ensuring window
  // created.
  void SetRect(const Rect& rect);

 protected:
  virtual gsl::not_null<platform::gui::INativeWindow*> CreateNativeWindow(
      gsl::not_null<host::WindowHost*> host,
      platform::gui::INativeWindow* parent) = 0;

 private:
  platform::gui::INativeWindow* GetNativeWindow(bool create);

 private:
  std::unique_ptr<host::WindowHost> window_host_;

  std::unique_ptr<render::StackLayoutRenderObject> render_object_;

  Control* attached_control_;
};
}  // namespace cru::ui::controls
