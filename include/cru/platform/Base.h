#pragma once

#include <cru/base/Base.h>

#include <string>

#include <optional>
#include <string_view>

#ifdef CRU_IS_DLL
#ifdef CRU_PLATFORM_EXPORT_API
#define CRU_PLATFORM_API __declspec(dllexport)
#else
#define CRU_PLATFORM_API __declspec(dllimport)
#endif
#else
#define CRU_PLATFORM_API
#endif


namespace cru::platform {
struct CRU_PLATFORM_API IPlatformResource : virtual Interface {
  virtual std::string GetPlatformId() const = 0;
  virtual std::string GetDebugString();
};

// This exception is thrown when a resource is used on another platform.
// Of course, you can't mix resources of two different platform.
// For example, Win32 Brush (may add in the future) with Direct Painter.
class CRU_PLATFORM_API PlatformNotMatchException : public PlatformException {
 public:
  PlatformNotMatchException(
      std::string resource_platform, std::string target_platform,
      std::optional<std::string_view> additional_message = std::nullopt);

  ~PlatformNotMatchException() override;

  std::string GetResourcePlatformUtf8() const { return resource_platform_; }
  std::string GetTargetPlatformUtf8() const { return target_platform_; }

 private:
  std::string resource_platform_;
  std::string target_platform_;
};

// This exception is thrown when a resource has been disposed and not usable
// again.
// For example, calling Build twice on a GeometryBuilder::Build will lead to
// this exception.
class CRU_PLATFORM_API ReuseException : public Exception {
 public:
  using Exception::Exception;  // inherit constructors

  CRU_DEFAULT_DESTRUCTOR(ReuseException)
};

class CRU_PLATFORM_API PlatformUnsupportedException : public PlatformException {
 public:
  PlatformUnsupportedException(
      std::string platform, std::string operation,
      std::optional<std::string_view> additional_message);

  ~PlatformUnsupportedException() override;

  std::string GetPlatformUtf8() const { return platform_; }
  std::string GetOperationUtf8() const { return operation_; }

 private:
  std::string platform_;
  std::string operation_;
};

template <typename TTarget>
TTarget* CheckPlatform(IPlatformResource* resource,
                       std::string target_platform) {
  if (resource == nullptr) return nullptr;
  const auto result = dynamic_cast<TTarget*>(resource);
  if (result == nullptr) {
    throw PlatformNotMatchException(
        resource->GetPlatformId(), target_platform,
        "Try to convert resource to target platform failed.");
  }
  return result;
}

template <typename TTarget, typename TSource>
std::shared_ptr<TTarget> CheckPlatform(const std::shared_ptr<TSource>& resource,
                                       std::string target_platform) {
  if (resource == nullptr) return nullptr;
  static_assert(std::is_base_of_v<IPlatformResource, TSource>,
                "TSource must be a subclass of IPlatformResource.");
  const auto result = std::dynamic_pointer_cast<TTarget>(resource);
  if (result == nullptr) {
    throw PlatformNotMatchException(
        resource->GetPlatformId(), target_platform,
        "Try to convert resource to target platform failed.");
  }
  return result;
}
}  // namespace cru::platform
