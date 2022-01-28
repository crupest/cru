#pragma once
#include "Base.hpp"

#include "cru/platform/graphics/Resource.hpp"

#include <string_view>

namespace cru::platform::graphics::win::direct {
class DirectGraphicsFactory;

class CRU_WIN_GRAPHICS_DIRECT_API DirectResource
    : public Object,
      public virtual IPlatformResource {
 public:
  static String kPlatformId;

 protected:
  DirectResource() = default;

 public:
  CRU_DELETE_COPY(DirectResource)
  CRU_DELETE_MOVE(DirectResource)

  ~DirectResource() override = default;

 public:
  String GetPlatformId() const final { return kPlatformId; }
};

class CRU_WIN_GRAPHICS_DIRECT_API DirectGraphicsResource
    : public DirectResource,
      public virtual IGraphicsResource {
 protected:
  // Param factory can't be null.
  explicit DirectGraphicsResource(DirectGraphicsFactory* factory);

 public:
  CRU_DELETE_COPY(DirectGraphicsResource)
  CRU_DELETE_MOVE(DirectGraphicsResource)

  ~DirectGraphicsResource() override = default;

 public:
  IGraphicsFactory* GetGraphicsFactory() final;

 public:
  DirectGraphicsFactory* GetDirectFactory() const { return factory_; }

 private:
  DirectGraphicsFactory* factory_;
};
}  // namespace cru::platform::graphics::win::direct
