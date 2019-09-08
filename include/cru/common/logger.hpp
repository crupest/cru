#pragma once

#include "cru/common/base.hpp"
#include "cru/common/format.hpp"

#include <iostream>
#include <list>
#include <string_view>

namespace cru::log {

enum class LogLevel { Debug, Info, Warn, Error };

struct ILoggerSource : Interface {
  // Write the s. LogLevel is just a helper. It has no effect on the content to
  // write.
  virtual void Write(LogLevel level, const std::wstring_view& s) = 0;
};

class StdioLoggerSource : public ILoggerSource {
 public:
  StdioLoggerSource() = default;

  CRU_DELETE_COPY(StdioLoggerSource)
  CRU_DELETE_MOVE(StdioLoggerSource)

  ~StdioLoggerSource() = default;

  void Write(LogLevel level, const std::wstring_view& s) override {
    if (level == LogLevel::Error) {
      std::wcerr << s;
    } else {
      std::wcout << s;
    }
  }
};

class Logger : public Object {
 public:
  static Logger* GetInstance();

 public:
  Logger() = default;

  CRU_DELETE_COPY(Logger)
  CRU_DELETE_MOVE(Logger)

  ~Logger() override;

 public:
  void AddSource(ILoggerSource* source);
  void RemoveSource(ILoggerSource* source);

 public:
  void Log(LogLevel level, const std::wstring_view& s);

  template <typename... TArgs>
  void Debug(const std::wstring_view& format, TArgs&&... args) {
    Log(LogLevel::Debug, util::Format(format, std::forward<TArgs>(args)...));
  }

  template <typename... TArgs>
  void Info(const std::wstring_view& format, TArgs&&... args) {
    Log(LogLevel::Info, util::Format(format, std::forward<TArgs>(args)...));
  }

  template <typename... TArgs>
  void Warn(const std::wstring_view& format, TArgs&&... args) {
    Log(LogLevel::Warn, util::Format(format, std::forward<TArgs>(args)...));
  }

  template <typename... TArgs>
  void Error(const std::wstring_view& format, TArgs&&... args) {
    Log(LogLevel::Error, util::Format(format, std::forward<TArgs>(args)...));
  }

 public:
  std::list<ILoggerSource*> sources_;
};

template <typename... TArgs>
void Debug(const std::wstring_view& format, TArgs&&... args) {
  Logger::GetInstance()->Log(
      LogLevel::Debug, util::Format(format, std::forward<TArgs>(args)...));
}

template <typename... TArgs>
void Info(const std::wstring_view& format, TArgs&&... args) {
  Logger::GetInstance()->Log(
      LogLevel::Info, util::Format(format, std::forward<TArgs>(args)...));
}

template <typename... TArgs>
void Warn(const std::wstring_view& format, TArgs&&... args) {
  Logger::GetInstance()->Log(
      LogLevel::Warn, util::Format(format, std::forward<TArgs>(args)...));
}

template <typename... TArgs>
void Error(const std::wstring_view& format, TArgs&&... args) {
  Logger::GetInstance()->Log(
      LogLevel::Error, util::Format(format, std::forward<TArgs>(args)...));
}
}  // namespace cru::logger
