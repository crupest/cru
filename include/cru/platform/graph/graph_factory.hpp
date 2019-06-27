#pragma once
#include "../graphic_base.hpp"
#include "../native_resource.hpp"

#include "brush.hpp"
#include "font.hpp"
#include "geometry.hpp"
#include "text_layout.hpp"

#include <memory>
#include <string>
#include <string_view>

namespace cru::platform::graph {
// Entry point of the graph module.
// If you create a IUiApplication instance, then you should not create
// IGraphFactory manually. IUiApplication will call
// IGraphFactory::CreateInstance and set auto-delete to true.
// The manual creation method of IGraphFactory provides a you a way to use graph
// related tools without interact with actual ui like window system.
class GraphFactory : public NativeResource {
 public:
  // Create a platform-specific instance and save it as the global instance.
  // Do not create the instance twice. Implements should assert for that.
  // After creating, get the instance by GetInstance.
  static GraphFactory* CreateInstance();

  // Get the global instance. If it is not created, then return nullptr.
  static GraphFactory* GetInstance();

 protected:
  GraphFactory() = default;

 public:
  GraphFactory(const GraphFactory& other) = delete;
  GraphFactory& operator=(const GraphFactory& other) = delete;

  GraphFactory(GraphFactory&& other) = delete;
  GraphFactory& operator=(GraphFactory&& other) = delete;

  ~GraphFactory() override = default;

 public:
  virtual SolidColorBrush* CreateSolidColorBrush() = 0;
  SolidColorBrush* CreateSolidColorBrush(const Color& color) {
    const auto brush = CreateSolidColorBrush();
    brush->SetColor(color);
    return brush;
  }

  virtual GeometryBuilder* CreateGeometryBuilder() = 0;

  virtual Font* CreateFont(const std::wstring_view& font_family,
                           float font_size) = 0;

  virtual TextLayout* CreateTextLayout(std::shared_ptr<Font> font,
                                       std::wstring text) = 0;

  virtual bool IsAutoDelete() const = 0;
  virtual void SetAutoDelete(bool value) = 0;
};
}  // namespace cru::platform::graph
