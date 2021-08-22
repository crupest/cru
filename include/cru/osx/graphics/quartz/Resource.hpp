#pragma once
#include "cru/osx/Resource.hpp"
#include "cru/platform/graphics/Base.hpp"
#include "cru/platform/graphics/Resource.hpp"

namespace cru::platform::graphics::osx::quartz {
class OsxQuartzResource : public platform::osx::OsxResource,
                          public virtual IGraphResource {
 public:
  explicit OsxQuartzResource(IGraphFactory* graphics_factory)
      : graphics_factory_(graphics_factory) {}

  CRU_DELETE_COPY(OsxQuartzResource)
  CRU_DELETE_MOVE(OsxQuartzResource)
  ~OsxQuartzResource() override = default;

 public:
  String GetPlatformId() const override { return u"OSX Quartz"; }

  IGraphFactory* GetGraphFactory() override;

 private:
  IGraphFactory* graphics_factory_;
};
}  // namespace cru::platform::graphics::osx::quartz
