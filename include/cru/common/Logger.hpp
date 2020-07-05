#pragma once
#include "cru/common/Base.hpp"

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <iostream>
#include <list>
#include <memory>
#include <string_view>

namespace cru::log {

enum class LogLevel { Debug, Info, Warn, Error };

struct ILogSource : virtual Interface {
  // Write the string s. LogLevel is just a helper. It has no effect on the
  // content to write.
  virtual void Write(LogLevel level, const std::u16string& s) = 0;
};

class Logger : public Object {
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
  void Log(LogLevel level, std::u16string_view s);
  void Log(LogLevel level, std::u16string_view tag, std::u16string_view s);

 public:
  std::list<std::unique_ptr<ILogSource>> sources_;
};

template <typename... TArgs>
void Debug([[maybe_unused]] TArgs&&... args) {
#ifdef CRU_DEBUG
  Logger::GetInstance()->Log(LogLevel::Debug,
                             fmt::format(std::forward<TArgs>(args)...));
#endif
}

template <typename... TArgs>
void Info(TArgs&&... args) {
  Logger::GetInstance()->Log(LogLevel::Info,
                             fmt::format(std::forward<TArgs>(args)...));
}

template <typename... TArgs>
void Warn(TArgs&&... args) {
  Logger::GetInstance()->Log(LogLevel::Warn,
                             fmt::format(std::forward<TArgs>(args)...));
}

template <typename... TArgs>
void Error(TArgs&&... args) {
  Logger::GetInstance()->Log(LogLevel::Error,
                             fmt::format(std::forward<TArgs>(args)...));
}

template <typename... TArgs>
void TagDebug([[maybe_unused]] std::u16string_view tag,
              [[maybe_unused]] TArgs&&... args) {
#ifdef CRU_DEBUG
  Logger::GetInstance()->Log(LogLevel::Debug, tag,
                             fmt::format(std::forward<TArgs>(args)...));
#endif
}

template <typename... TArgs>
void TagInfo(std::u16string_view tag, TArgs&&... args) {
  Logger::GetInstance()->Log(LogLevel::Info, tag,
                             fmt::format(std::forward<TArgs>(args)...));
}

template <typename... TArgs>
void TagWarn(std::u16string_view tag, TArgs&&... args) {
  Logger::GetInstance()->Log(LogLevel::Warn, tag,
                             fmt::format(std::forward<TArgs>(args)...));
}

template <typename... TArgs>
void TagError(std::u16string_view tag, TArgs&&... args) {
  Logger::GetInstance()->Log(LogLevel::Error, tag,
                             fmt::format(std::forward<TArgs>(args)...));
}
}  // namespace cru::log
