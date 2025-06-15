#pragma once
#include "Base.h"

#include "cru/graphics/Resource.h"

#include <string_view>

namespace cru::graphics::direct2d {
class DirectGraphicsFactory;

class CRU_WIN_GRAPHICS_DIRECT_API DirectResource
    : public Object,
      public virtual IPlatformResource {
 public:
  static String kPlatformId;

 protected:
  DirectResource() = default;

 public:

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

  ~DirectGraphicsResource() override = default;

 public:
  IGraphicsFactory* GetGraphicsFactory() final;

 public:
  DirectGraphicsFactory* GetDirectFactory() const { return factory_; }

 private:
  DirectGraphicsFactory* factory_;
};
}  // namespace cru::graphics::direct2d
