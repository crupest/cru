#pragma once
#include "UiApplication.h"

#include <cru/base/Guard.h>
#include <memory>
#include <utility>

namespace cru::platform::gui {
template <typename TSelf>
class DeleteLaterImpl {
  CRU_DEFINE_CLASS_LOG_TAG("cru::platform::gui::DeleteLaterImpl")

 public:
  virtual ~DeleteLaterImpl() {}

  void DeleteLater() {
    IUiApplication::GetInstance()->DeleteLater(static_cast<TSelf*>(this));
  }
};

namespace details {
template <typename T>
struct DeleteLaterPtrDeleter {
  void operator()(T* p) const noexcept { p->DeleteLater(); }
};
}  // namespace details

template <typename T>
using DeleteLaterPtr = std::unique_ptr<T, details::DeleteLaterPtrDeleter<T>>;

template <typename T>
DeleteLaterPtr<T> ToDeleteLaterPtr(std::unique_ptr<T>&& p) {
  return DeleteLaterPtr<T>(p.release());
}

template <typename T, typename... Args>
DeleteLaterPtr<T> MakeDeleteLater(Args&&... args) {
  return DeleteLaterPtr<T>(new T(std::forward<Args>(args)...));
}

}  // namespace cru::platform::gui
