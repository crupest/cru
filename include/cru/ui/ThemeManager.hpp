#pragma once
#include "Base.hpp"
#include "cru/common/Base.hpp"
#include "cru/common/Event.hpp"

#include <boost/property_tree/ptree.hpp>
#include <cstddef>

namespace cru::ui {
class ThemeManager : public Object {
 public:
  ThemeManager();

  CRU_DELETE_COPY(ThemeManager)
  CRU_DELETE_MOVE(ThemeManager)

  ~ThemeManager() override;

  IEvent<std::nullptr_t>* ThemeResourceChangeEvent() {
    return &theme_resource_change_event_;
  }

 private:
  Event<std::nullptr_t> theme_resource_change_event_;
  boost::property_tree::iptree theme_tree_;
};
}  // namespace cru::ui
