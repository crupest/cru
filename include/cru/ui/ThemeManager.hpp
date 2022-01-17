#pragma once
#include "Base.hpp"
#include "cru/common/Base.hpp"
#include "cru/common/Event.hpp"
#include "cru/common/Exception.hpp"
#include "cru/platform/graphics/Brush.hpp"

#include <unordered_map>

namespace cru::ui {
class CRU_UI_API ThemeResourceKeyNotExistException : public Exception {
 public:
  using Exception::Exception;
};

class CRU_UI_API BadThemeResourceException : public Exception {
 public:
  using Exception::Exception;
};

class CRU_UI_API ThemeManager : public Object {
 public:
  static ThemeManager* GetInstance();

 private:
  ThemeManager();

 public:
  CRU_DELETE_COPY(ThemeManager)
  CRU_DELETE_MOVE(ThemeManager)

  ~ThemeManager() override = default;

  IEvent<std::nullptr_t>* ThemeResourceChangeEvent() {
    return &theme_resource_change_event_;
  }

  gsl::not_null<std::shared_ptr<platform::graphics::IBrush>> GetBrush(
      StringView key);

 private:
  void Init();

 private:
  Event<std::nullptr_t> theme_resource_change_event_;
  std::unordered_map<String, String> theme_resource_map_;
  std::unordered_map<String, std::shared_ptr<platform::graphics::IBrush>>
      brushes_;
};
}  // namespace cru::ui
