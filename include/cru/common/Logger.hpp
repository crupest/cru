#pragma once
#include "Base.hpp"

#include "String.hpp"

#include <list>
#include <memory>

namespace cru::log {

enum class LogLevel { Debug, Info, Warn, Error };

struct CRU_BASE_API ILogSource : virtual Interface {
  // Write the string s. LogLevel is just a helper. It has no effect on the
  // content to write.
  virtual void Write(LogLevel level, const String& s) = 0;
};

class CRU_BASE_API Logger : public Object {
 public:
  static Logger* GetInstance();

 public:
  Logger() = default;

  CRU_DELETE_COPY(Logger)
  CRU_DELETE_MOVE(Logger)

  ~Logger() override = default;

 public:
  void AddSource(std::unique_ptr<ILogSource> source);
  void RemoveSource(ILogSource* source);

 public:
  void Log(LogLevel level, const String& message);
  void Log(LogLevel level, const String& tag, const String& message);

 private:
  std::list<std::unique_ptr<ILogSource>> sources_;
};

// TODO: Remove argument evaluation in Debug.
template <typename... TArgs>
void Debug([[maybe_unused]] TArgs&&... args) {
#ifdef CRU_DEBUG
  Logger::GetInstance()->Log(LogLevel::Debug,
                             Format(std::forward<TArgs>(args)...));
#endif
}

template <typename... TArgs>
void Info(TArgs&&... args) {
  Logger::GetInstance()->Log(LogLevel::Info,
                             Format(std::forward<TArgs>(args)...));
}

template <typename... TArgs>
void Warn(TArgs&&... args) {
  Logger::GetInstance()->Log(LogLevel::Warn,
                             Format(std::forward<TArgs>(args)...));
}

template <typename... TArgs>
void Error(TArgs&&... args) {
  Logger::GetInstance()->Log(LogLevel::Error,
                             Format(std::forward<TArgs>(args)...));
}

// TODO: Remove argument evaluation in Debug.
template <typename... TArgs>
void TagDebug([[maybe_unused]] std::u16string_view tag,
              [[maybe_unused]] TArgs&&... args) {
#ifdef CRU_DEBUG
  Logger::GetInstance()->Log(LogLevel::Debug, tag,
                             Format(std::forward<TArgs>(args)...));
#endif
}

template <typename... TArgs>
void TagInfo(std::u16string_view tag, TArgs&&... args) {
  Logger::GetInstance()->Log(LogLevel::Info, tag,
                             Format(std::forward<TArgs>(args)...));
}

template <typename... TArgs>
void TagWarn(std::u16string_view tag, TArgs&&... args) {
  Logger::GetInstance()->Log(LogLevel::Warn, tag,
                             Format(std::forward<TArgs>(args)...));
}

template <typename... TArgs>
void TagError(std::u16string_view tag, TArgs&&... args) {
  Logger::GetInstance()->Log(LogLevel::Error, tag,
                             Format(std::forward<TArgs>(args)...));
}
}  // namespace cru::log
