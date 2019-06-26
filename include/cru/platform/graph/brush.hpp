#pragma once
#include "../graphic_base.hpp"
#include "../native_resource.hpp"

namespace cru::platform::graph {
class Brush : public NativeResource {
 protected:
  Brush() = default;

 public:
  Brush(const Brush& other) = delete;
  Brush& operator=(const Brush& other) = delete;

  Brush(Brush&& other) = delete;
  Brush& operator=(Brush&& other) = delete;

  ~Brush() override = default;
};

class SolidColorBrush : public Brush {
 protected:
  SolidColorBrush() = default;

 public:
  SolidColorBrush(const SolidColorBrush& other) = delete;
  SolidColorBrush& operator=(const SolidColorBrush& other) = delete;

  SolidColorBrush(SolidColorBrush&& other) = delete;
  SolidColorBrush& operator=(SolidColorBrush&& other) = delete;

  ~SolidColorBrush() = default;

 public:
  Color GetColor() { return color_; }
  void SetColor(const Color& color) {
    color_ = color;
    OnSetColor(color);
  }

 protected:
  virtual void OnSetColor(const Color& color) = 0;

 protected:
  Color color_ = colors::black;
};
}  // namespace cru::platform::graph
