#pragma once
#include "../Base.h"

#include <condition_variable>
#include <format>  // IWYU pragma: keep
#include <list>
#include <memory>
#include <mutex>
#include <string_view>
#include <thread>
#include <unordered_set>
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

struct CRU_BASE_API ILogger : virtual Interface {
  static ILogger* GetInstance();

  virtual void AddDebugTag(std::string tag) = 0;
  virtual void RemoveDebugTag(const std::string& tag) = 0;
  void LoadDebugTagFromEnv(const char* env_var = "CRU_LOG_DEBUG",
                           std::string sep = ",");

  virtual void Log(LogInfo log_info) = 0;

  void Log(LogLevel level, std::string tag, std::string message) {
    Log(LogInfo(level, std::move(tag), std::move(message)));
  }

  template <typename... Args>
  void FormatLog(LogLevel level, std::string tag,
                 std::string_view message_format, Args&&... args) {
    // Clang is buggy in consteval, so we can't use std::format for now.
    Log(level, std::move(tag),
        std::vformat(message_format, std::make_format_args(args...)));
  }

 protected:
  std::unordered_set<std::string> debug_tags_;
};

class CRU_BASE_API SimpleStdioLogger : public Object, public virtual ILogger {
 public:
  void AddDebugTag(std::string tag) override;
  void RemoveDebugTag(const std::string& tag) override;
  void Log(LogInfo log_info) override;

 private:
  std::unordered_set<std::string> debug_tags_;
};

class CRU_BASE_API Logger : public Object, public virtual ILogger {
 public:
  Logger();
  ~Logger() override;

 public:
  void AddDebugTag(std::string tag) override;
  void RemoveDebugTag(const std::string& tag) override;

  void AddLogTarget(std::unique_ptr<ILogTarget> source);
  void RemoveLogTarget(ILogTarget* source);

 public:
  void Log(LogInfo log_info) override;

 private:
  void LogThreadRun();

 private:
  std::mutex log_queue_mutex_;
  std::unordered_set<std::string> debug_tags_;
  std::condition_variable log_queue_condition_variable_;
  std::list<LogInfo> log_queue_;
  bool log_stop_;
  std::thread log_thread_;

  std::mutex target_list_mutex_;
  std::vector<std::unique_ptr<ILogTarget>> target_list_;
};

class CRU_BASE_API LoggerCppStream : public Object {
 public:
  explicit LoggerCppStream(ILogger* logger, LogLevel level, std::string tag);

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
  ILogger* logger_;
  LogLevel level_;
  std::string tag_;
};

}  // namespace cru::log

#define CRU_LOG_TAG_DEBUG(...)                                           \
  cru::log::ILogger::GetInstance()->FormatLog(cru::log::LogLevel::Debug, \
                                              kLogTag, __VA_ARGS__)

#define CRU_LOG_TAG_INFO(...)                                           \
  cru::log::ILogger::GetInstance()->FormatLog(cru::log::LogLevel::Info, \
                                              kLogTag, __VA_ARGS__)

#define CRU_LOG_TAG_WARN(...)                                           \
  cru::log::ILogger::GetInstance()->FormatLog(cru::log::LogLevel::Warn, \
                                              kLogTag, __VA_ARGS__)

#define CRU_LOG_TAG_ERROR(...)                                           \
  cru::log::ILogger::GetInstance()->FormatLog(cru::log::LogLevel::Error, \
                                              kLogTag, __VA_ARGS__)
