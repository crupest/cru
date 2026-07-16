#pragma once

#include <cru/platform/gui/Base.h>

#include <string>

#ifdef CRU_IS_DLL
#ifdef CRU_PLATFORM_GUI_MOCK_EXPORT_API
#define CRU_PLATFORM_GUI_MOCK_API __declspec(dllexport)
#else
#define CRU_PLATFORM_GUI_MOCK_API __declspec(dllimport)
#endif
#else
#define CRU_PLATFORM_GUI_MOCK_API
#endif

namespace cru::platform::gui::mock {
class CRU_PLATFORM_GUI_MOCK_API MockResource
    : public Object,
      public virtual IPlatformResource {
 public:
  static constexpr const char* kPlatformId = "Mock";
  MockResource() = default;

 public:
  std::string GetPlatformId() const final { return kPlatformId; }
};

std::string CRU_PLATFORM_GUI_MOCK_API GetMockPlatformId();
}  // namespace cru::platform::gui::mock
