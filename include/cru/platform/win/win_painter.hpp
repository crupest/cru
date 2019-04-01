#pragma once
#include "win_pre_config.hpp"

#include "../painter.hpp"

namespace cru::platform::win {
class WinNativeWindow;

class WinPainter : public Object, public virtual Painter {
 public:
  explicit WinPainter(WinNativeWindow* window);
  WinPainter(const WinPainter& other) = delete;
  WinPainter(WinPainter&& other) = delete;
  WinPainter& operator=(const WinPainter& other) = delete;
  WinPainter& operator=(WinPainter&& other) = delete;
  ~WinPainter() override;

  Matrix GetTransform() override;
  void SetTransform(const Matrix& matrix) override;
  void StrokeGeometry(Geometry* geometry, Brush* brush, float width) override;
  void FillGeometry(Geometry* geometry, Brush* brush) override;
  void EndDraw() override;
  bool IsDisposed() override { return is_disposed; }

  void EndDrawAndDeleteThis() {
    EndDraw();
    delete this;
  }

 private:
  WinNativeWindow* window_;
  ID2D1RenderTarget* render_target_;

  bool is_disposed = false;
};
}  // namespace cru::platform::win
