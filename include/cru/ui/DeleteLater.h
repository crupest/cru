#pragma once
#include "Base.h"

#include <cru/base/Guard.h>
#include <memory>
#include <utility>

namespace cru::ui {
class CRU_UI_API DeleteLaterImpl {
  CRU_DEFINE_CLASS_LOG_TAG("cru::ui::DeleteLaterImpl")
 public:
  DeleteLaterImpl();
  virtual ~DeleteLaterImpl();
  void DeleteLater();

 private:
  bool delete_later_scheduled_;
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

}  // namespace cru::ui
