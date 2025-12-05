#pragma once
#include "../Base.h"

#include <condition_variable>
#include <format>
#include <list>
#include <memory>
#include <mutex>
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
}  // namespace cru::log

#define CRU_DEFINE_LOG_FUNC(level)                                             \
  template <typename... Args>                                                  \
  void CruLog##level(std::string tag, std::format_string<Args...> message_fmt, \
                     Args&&... args) {                                         \
    cru::log::ILogger::GetInstance()->Log(                                     \
        cru::log::LogLevel::level, std::move(tag),                             \
        std::format(message_fmt, std::forward<Args>(args)...));                \
  }

CRU_DEFINE_LOG_FUNC(Debug)
CRU_DEFINE_LOG_FUNC(Info)
CRU_DEFINE_LOG_FUNC(Warn)
CRU_DEFINE_LOG_FUNC(Error)

#undef CRU_DEFINE_LOG_FUNC