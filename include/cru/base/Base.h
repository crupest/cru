#pragma once
#include <cassert>
#include <cstddef>
#include <exception>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#ifdef CRU_IS_DLL
#ifdef CRU_BASE_EXPORT_API
#define CRU_BASE_API __declspec(dllexport)
#else
#define CRU_BASE_API __declspec(dllimport)
#endif
#else
#define CRU_BASE_API
#endif

#define CRU_UNUSED(entity) static_cast<void>(entity);

#define CRU_DEFAULT_COPY(classname)      \
  classname(const classname&) = default; \
  classname& operator=(const classname&) = default;

#define CRU_DEFAULT_MOVE(classname) \
  classname(classname&&) = default; \
  classname& operator=(classname&&) = default;

#define CRU_DELETE_COPY(classname)      \
  classname(const classname&) = delete; \
  classname& operator=(const classname&) = delete;

#define CRU_DELETE_MOVE(classname) \
  classname(classname&&) = delete; \
  classname& operator=(classname&&) = delete;

#define CRU_DEFAULT_DESTRUCTOR(classname) ~classname() = default;

#define CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(classname) \
  classname() = default;                              \
  ~classname() = default;

#define CRU_DEFINE_COMPARE_OPERATORS(classname)                           \
  inline bool operator==(const classname& left, const classname& right) { \
    return left.Compare(right) == 0;                                      \
  }                                                                       \
                                                                          \
  inline bool operator!=(const classname& left, const classname& right) { \
    return left.Compare(right) != 0;                                      \
  }                                                                       \
                                                                          \
  inline bool operator<(const classname& left, const classname& right) {  \
    return left.Compare(right) < 0;                                       \
  }                                                                       \
                                                                          \
  inline bool operator<=(const classname& left, const classname& right) { \
    return left.Compare(right) <= 0;                                      \
  }                                                                       \
                                                                          \
  inline bool operator>(const classname& left, const classname& right) {  \
    return left.Compare(right) > 0;                                       \
  }                                                                       \
                                                                          \
  inline bool operator>=(const classname& left, const classname& right) { \
    return left.Compare(right) >= 0;                                      \
  }

namespace cru {
class CRU_BASE_API Object {
 public:
  Object() = default;
  CRU_DELETE_COPY(Object)
  CRU_DELETE_MOVE(Object)
  virtual ~Object() = default;
};

struct CRU_BASE_API Interface {
  Interface() = default;
  CRU_DELETE_COPY(Interface)
  CRU_DELETE_MOVE(Interface)
  virtual ~Interface() = default;
};

[[noreturn]] void CRU_BASE_API UnreachableCode();
[[noreturn]] void CRU_BASE_API NotImplemented();

using Index = std::ptrdiff_t;

inline void Expects(bool condition) { assert(condition); }
inline void Ensures(bool condition) { assert(condition); }
template <typename T>
inline void Expects(const std::shared_ptr<T>& ptr) {
  assert(ptr != nullptr);
}
template <typename T>
inline void Ensures(const std::shared_ptr<T>& ptr) {
  assert(ptr != nullptr);
}

// https://www.boost.org/doc/libs/1_54_0/doc/html/hash/reference.html#boost.hash_combine
template <class T>
inline void hash_combine(std::size_t& s, const T& v) {
  std::hash<T> h;
  s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
}

#define CRU_DEFINE_CLASS_LOG_TAG(tag) \
 private:                             \
  constexpr static const char* kLogTag = tag;

#ifdef _MSC_VER
#pragma warning(disable : 4275)
#endif
class CRU_BASE_API Exception : public std::exception {
 public:
  explicit Exception(std::string message = "",
                     std::shared_ptr<std::exception> inner = nullptr);

  ~Exception() override;

 public:
  std::string GetUtf8Message() const { return this->message_; }

  std::exception* GetInner() const noexcept { return inner_.get(); }

  const char* what() const noexcept override;

 protected:
  void SetMessage(std::string message) { message_ = std::move(message); }
  void AppendMessage(const std::string& additional_message);
  void AppendMessage(std::string_view additional_message);
  void AppendMessage(std::optional<std::string_view> additional_message);

 private:
  std::string message_;
  std::shared_ptr<std::exception> inner_;
};

class CRU_BASE_API PlatformException : public Exception {
 public:
  using Exception::Exception;  // inherit constructors
};

class ErrnoException : public Exception {
 public:
  ErrnoException();
  explicit ErrnoException(int error_code);
  /**
   * @brief will retrieve errno automatically.
   */
  explicit ErrnoException(std::string_view message);
  ErrnoException(std::string_view message, int errno_code);

  int GetErrnoCode() const { return errno_code_; }

 private:
  int errno_code_;
};
}  // namespace cru
