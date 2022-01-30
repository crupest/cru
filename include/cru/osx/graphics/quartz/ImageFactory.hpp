#pragma once
#include "Resource.hpp"
#include "cru/platform/graphics/ImageFactory.hpp"

namespace cru::platform::graphics::osx::quartz {
class QuartzImageFactory : public OsxQuartzResource,
                           public virtual IImageFactory {
 public:
  explicit QuartzImageFactory(IGraphicsFactory* graphics_factory);

  CRU_DELETE_COPY(QuartzImageFactory)
  CRU_DELETE_MOVE(QuartzImageFactory)

  ~QuartzImageFactory() override;

 public:
  std::unique_ptr<IImage> DecodeFromStream(io::Stream* stream) override;
};
}  // namespace cru::platform::graphics::osx::quartz
