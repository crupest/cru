#pragma once

#include "Base.h"
#include "Exception.h"
#include "String.h"

namespace cru {
struct CRU_BASE_API IPlatformResource : virtual Interface {
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(IPlatformResource)

  virtual String GetPlatformId() const = 0;

  virtual String GetDebugString() { return String(); }
};

class CRU_BASE_API PlatformException : public Exception {
 public:
  using Exception::Exception;  // inherit constructors

  CRU_DEFAULT_DESTRUCTOR(PlatformException)
};

// This exception is thrown when a resource is used on another platform.
// Of course, you can't mix resources of two different platform.
// For example, Win32 Brush (may add in the future) with Direct Painter.
class CRU_BASE_API PlatformNotMatchException : public PlatformException {
 public:
  PlatformNotMatchException(
      String resource_platform, String target_platform,
      std::optional<StringView> additional_message = std::nullopt);

  ~PlatformNotMatchException() override;

  String GetResourcePlatform() const;
  String GetTargetPlatform() const;

 private:
  String resource_platform_;
  String target_platform_;
};

class CRU_BASE_API PlatformUnsupportedException : public PlatformException {
 public:
  PlatformUnsupportedException(String platform, String operation,
                               std::optional<StringView> additional_message);

  ~PlatformUnsupportedException() override;

  String GetPlatform() const;
  String GetOperation() const;

 private:
  String platform_;
  String operation_;
};

template <typename TTarget>
TTarget* CheckPlatform(IPlatformResource* resource,
                       const String& target_platform) {
  if (resource == nullptr) return nullptr;
  const auto result = dynamic_cast<TTarget*>(resource);
  if (result == nullptr) {
    throw PlatformNotMatchException(
        resource->GetPlatformId(), target_platform,
        u"Try to convert resource to target platform failed.");
  }
  return result;
}

template <typename TTarget, typename TSource>
std::shared_ptr<TTarget> CheckPlatform(const std::shared_ptr<TSource>& resource,
                                       const String& target_platform) {
  if (resource == nullptr) return nullptr;
  static_assert(std::is_base_of_v<IPlatformResource, TSource>,
                "TSource must be a subclass of INativeResource.");
  const auto result = std::dynamic_pointer_cast<TTarget>(resource);
  if (result == nullptr) {
    throw PlatformNotMatchException(
        resource->GetPlatformId(), target_platform,
        u"Try to convert resource to target platform failed.");
  }
  return result;
}
}  // namespace cru

#ifdef __APPLE__
namespace cru {
class OsxResource : public Object, public virtual IPlatformResource {
 public:
  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(OsxResource)

  String GetPlatformId() const override { return u"OSX"; }
};
}  // namespace cru
#endif
