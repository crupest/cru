#pragma once
#include "cru/platform/graphics/Base.h"
#include "cru/platform/graphics/Resource.h"
#include "cru/platform/osx/Resource.h"

namespace cru::platform::graphics::quartz {
class OsxQuartzResource : public platform::osx::OsxResource,
                          public virtual IGraphicsResource {
 public:
  explicit OsxQuartzResource(IGraphicsFactory* graphics_factory)
      : graphics_factory_(graphics_factory) {}

  CRU_DELETE_COPY(OsxQuartzResource)
  CRU_DELETE_MOVE(OsxQuartzResource)
  ~OsxQuartzResource() override = default;

 public:
  std::string GetPlatformId() const override { return "OSX Quartz"; }

  IGraphicsFactory* GetGraphicsFactory() override { return graphics_factory_; }

 private:
  IGraphicsFactory* graphics_factory_;
};
}  // namespace cru::platform::graphics::quartz
