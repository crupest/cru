#pragma once
#include "../../WinPreConfig.hpp"

#include "cru/platform/graphics/Resource.hpp"

#include <string_view>

namespace cru::platform::graphics::win::direct {
class DirectGraphFactory;

class DirectResource : public Object, public virtual INativeResource {
 public:
  static constexpr std::u16string_view k_platform_id = u"Windows Direct";

 protected:
  DirectResource() = default;

 public:
  CRU_DELETE_COPY(DirectResource)
  CRU_DELETE_MOVE(DirectResource)

  ~DirectResource() override = default;

 public:
  std::u16string_view GetPlatformId() const final { return k_platform_id; }
};

class DirectGraphResource : public DirectResource,
                            public virtual IGraphResource {
 protected:
  // Param factory can't be null.
  explicit DirectGraphResource(DirectGraphFactory* factory);

 public:
  CRU_DELETE_COPY(DirectGraphResource)
  CRU_DELETE_MOVE(DirectGraphResource)

  ~DirectGraphResource() override = default;

 public:
  IGraphFactory* GetGraphFactory() final;

 public:
  DirectGraphFactory* GetDirectFactory() const { return factory_; }

 private:
  DirectGraphFactory* factory_;
};
}  // namespace cru::platform::graphics::win::direct
