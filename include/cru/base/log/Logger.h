#pragma once
#include "../Base.h"
#include "../Guard.h"

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

struct CRU_BASE_API ILogFormatter : virtual Interface {
  virtual std::string Format(const LogInfo& log_info) = 0;
};

struct CRU_BASE_API DefaultLogFormatter : public Object,
                                          public virtual ILogFormatter {
  std::string Format(const LogInfo& log_info) override;
};

struct CRU_BASE_API ILogWriter : virtual Interface {
  virtual void Write(const LogInfo& log_info, std::string log_str) = 0;
};

struct CRU_BASE_API ILogger : virtual Interface {
  static ILogger* GetInstance();
  static void InstallDefaultWriters(ILogger* logger);

  virtual void AddDebugTag(std::string tag) = 0;
  virtual void RemoveDebugTag(const std::string& tag) = 0;
  void LoadDebugTagFromEnv(const char* env_var = "CRU_LOG_DEBUG",
                           std::string sep = ",");

  virtual void SetFormatter(std::unique_ptr<ILogFormatter> formatter) = 0;
  virtual void AddWriter(std::unique_ptr<ILogWriter> source) = 0;
  virtual void RemoveWriter(ILogWriter* source) = 0;

  virtual void Log(LogInfo log_info) = 0;

  void Log(LogLevel level, std::string tag, std::string message) {
    Log(LogInfo(level, std::move(tag), std::move(message)));
  }
};

class CRU_BASE_API LoggerConfigurationMixin : public virtual ILogger {
 public:
  LoggerConfigurationMixin();

  void AddDebugTag(std::string tag) override;
  void RemoveDebugTag(const std::string& tag) override;
  void SetFormatter(std::unique_ptr<ILogFormatter> formatter) override;
  void AddWriter(std::unique_ptr<ILogWriter> writer) override;
  void RemoveWriter(ILogWriter* writer) override;

 protected:
  struct LoggerConfig {
    std::shared_ptr<ILogFormatter> formatter;
    std::unordered_set<std::string> debug_tags;
    std::vector<std::shared_ptr<ILogWriter>> writers;
  };

  std::shared_ptr<LoggerConfig> GetConfigSnapshot();

 private:
  template <typename F>
  void UpdateConfig(F&& updater) {
    auto old_config = config_.load();
    while (true) {
      std::shared_ptr<LoggerConfig> new_config(
          new LoggerConfig(*old_config.get()));
      updater(new_config.get());
      if (config_.compare_exchange_weak(old_config, new_config)) {
        break;
      }
    }
  }

  std::atomic<std::shared_ptr<LoggerConfig>> config_;
};

class CRU_BASE_API SynchronousLogger : public Object,
                                       public virtual ILogger,
                                       public LoggerConfigurationMixin {
  void Log(LogInfo log_info) override;
};

class CRU_BASE_API AsynchronousLogger : public Object,
                                        public virtual ILogger,
                                        public LoggerConfigurationMixin {
 public:
  AsynchronousLogger();
  ~AsynchronousLogger() override;

 public:
  void Log(LogInfo log_info) override;

 private:
  void LogThreadRun();

 private:
  std::mutex mutex_;
  std::condition_variable condition_variable_;

  std::list<LogInfo> log_queue_;
  bool log_stop_;
  std::thread log_thread_;
};

CRU_BASE_API Guard MeasureTimeAndLog(std::string_view tag,
                                     std::string_view name);
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
