#pragma once
#include "Base.h"

#include "../Resource.h"

namespace cru::graphics::cairo {
class CairoGraphicsFactory;

class CRU_PLATFORM_GRAPHICS_CAIRO_API CairoResource
    : public Object,
      public virtual IGraphicsResource {
 public:
  explicit CairoResource(CairoGraphicsFactory* factory);


  ~CairoResource() override;

  String GetPlatformId() const override;
  IGraphicsFactory* GetGraphicsFactory() override;
  CairoGraphicsFactory* GetCairoGraphicsFactory() const { return factory_; }

 private:
  CairoGraphicsFactory* factory_;
};
}  // namespace cru::graphics::cairo
