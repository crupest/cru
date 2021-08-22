#pragma once
#include "Resource.hpp"
#include "cru/common/Base.hpp"
#include "cru/platform/graphics/Factory.hpp"

namespace cru::platform::graphics::osx::quartz {
class QuartzGraphicsFactory : public OsxQuartzResource,
                              public virtual IGraphFactory {
 public:
  QuartzGraphicsFactory() : OsxQuartzResource(this) {}

  CRU_DELETE_COPY(QuartzGraphicsFactory)
  CRU_DELETE_MOVE(QuartzGraphicsFactory)

  ~QuartzGraphicsFactory() override = default;

 public:
  std::unique_ptr<ISolidColorBrush> CreateSolidColorBrush() override;
};
}  // namespace cru::platform::graphics::osx::quartz
