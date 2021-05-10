#pragma once
#include "Base.hpp"
#include "cru/common/Base.hpp"
#include "cru/common/Event.hpp"
#include "cru/platform/graphics/Brush.hpp"

#include <boost/property_tree/ptree.hpp>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <unordered_map>

namespace cru::ui {
class BadThemeResourceException : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};

class ThemeManager : public Object {
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
      std::u16string key);

 private:
  void Init();

 private:
  Event<std::nullptr_t> theme_resource_change_event_;
  boost::property_tree::ptree theme_tree_;
  std::unordered_map<std::u16string,
                     std::shared_ptr<platform::graphics::IBrush>>
      brushes_;
};
}  // namespace cru::ui
