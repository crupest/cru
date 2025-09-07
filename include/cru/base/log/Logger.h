#pragma once
#include "../Base.h"

#include <condition_variable>
#include <format>  // IWYU pragma: keep
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

namespace cru::log {
enum class LogLevel { Debug, Info, Warn, Error };

struct CRU_BASE_API LogInfo {
  LogInfo(LogLevel level, std::string tag, std::string message)
      : level(level), tag(std::move(tag)), message(std::move(message)) {}

  LogLevel level;
  std::string tag;
  std::string message;
};

struct CRU_BASE_API ILogTarget : virtual Interface {
  // Write the string s. LogLevel is just a helper. It has no effect on the
  // content to write.
  virtual void Write(LogLevel level, std::string s) = 0;
};

class CRU_BASE_API Logger : public Object2 {
 public:
  static Logger* GetInstance();

 public:
  Logger();
  ~Logger() override;

 public:
  void AddLogTarget(std::unique_ptr<ILogTarget> source);
  void RemoveLogTarget(ILogTarget* source);

 public:
  void Log(LogLevel level, std::string tag, std::string message) {
    Log(LogInfo(level, std::move(tag), std::move(message)));
  }
  void Log(LogInfo log_info);

  template <typename... Args>
  void FormatLog(LogLevel level, std::string tag,
                 std::string_view message_format, Args&&... args) {
    // Clang is buggy in consteval, so we can't use std::format for now.
    Log(level, std::move(tag),
        std::vformat(message_format, std::make_format_args(args...)));
  }

 private:
  void LogThreadRun();

 private:
  std::mutex log_queue_mutex_;
  std::condition_variable log_queue_condition_variable_;
  std::list<LogInfo> log_queue_;
  bool log_stop_;
  std::thread log_thread_;

  std::mutex target_list_mutex_;
  std::vector<std::unique_ptr<ILogTarget>> target_list_;
};

class CRU_BASE_API LoggerCppStream : public Object2 {
 public:
  explicit LoggerCppStream(Logger* logger, LogLevel level, std::string tag);
  ~LoggerCppStream() override = default;

  LoggerCppStream WithLevel(LogLevel level) const;
  LoggerCppStream WithTag(std::string tag) const;

 private:
  void Consume(std::string_view str);

 public:
  LoggerCppStream& operator<<(std::string_view str) {
    this->Consume(str);
    return *this;
  }

  template <typename T>
  LoggerCppStream& operator<<(T&& arg) {
    this->Consume(ToString(std::forward<T>(arg)));
    return *this;
  }

 private:
  Logger* logger_;
  LogLevel level_;
  std::string tag_;
};

}  // namespace cru::log

#define CRU_LOG_TAG_DEBUG(...)                                          \
  cru::log::Logger::GetInstance()->FormatLog(cru::log::LogLevel::Debug, \
                                             kLogTag, __VA_ARGS__)

#define CRU_LOG_TAG_INFO(...)                                          \
  cru::log::Logger::GetInstance()->FormatLog(cru::log::LogLevel::Info, \
                                             kLogTag, __VA_ARGS__)

#define CRU_LOG_TAG_WARN(...)                                          \
  cru::log::Logger::GetInstance()->FormatLog(cru::log::LogLevel::Warn, \
                                             kLogTag, __VA_ARGS__)

#define CRU_LOG_TAG_ERROR(...)                                          \
  cru::log::Logger::GetInstance()->FormatLog(cru::log::LogLevel::Error, \
                                             kLogTag, __VA_ARGS__)
