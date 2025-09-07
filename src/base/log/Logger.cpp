#include "cru/base/log/Logger.h"
#include "cru/base/log/StdioLogTarget.h"

#include <algorithm>
#include <condition_variable>
#include <ctime>
#include <format>
#include <memory>
#include <mutex>

#ifdef CRU_PLATFORM_WINDOWS
#include "cru/base/platform/win/DebugLogTarget.h"
#endif

namespace cru::log {
Logger *Logger::GetInstance() {
  static Logger logger;

  logger.AddLogTarget(std::make_unique<StdioLogTarget>());

#ifdef CRU_PLATFORM_WINDOWS
  logger.AddLogTarget(std::make_unique<platform::win::WinDebugLogTarget>());
#endif

  return &logger;
}

void Logger::AddLogTarget(std::unique_ptr<ILogTarget> target) {
  std::lock_guard<std::mutex> lock(target_list_mutex_);
  target_list_.push_back(std::move(target));
}

void Logger::RemoveLogTarget(ILogTarget *target) {
  std::lock_guard<std::mutex> lock(target_list_mutex_);
  target_list_.erase(
      std::remove_if(target_list_.begin(), target_list_.end(),
                     [target](const auto &t) { return t.get() == target; }),
      target_list_.end());
}

namespace {
const char *LogLevelToString(LogLevel level) {
  switch (level) {
    case LogLevel::Debug:
      return "DEBUG";
    case LogLevel::Info:
      return "INFO";
    case LogLevel::Warn:
      return "WARN";
    case LogLevel::Error:
      return "ERROR";
    default:
      std::terminate();
  }
}

std::string GetLogTime() {
  auto time = std::time(nullptr);
  auto calendar = std::localtime(&time);
  return std::format("{}:{}:{}", calendar->tm_hour, calendar->tm_min,
                     calendar->tm_sec);
}

std::string MakeLogFinalMessage(const LogInfo &log_info) {
  return std::format("[{}] {} {}: {}\n", GetLogTime(),
                     LogLevelToString(log_info.level), log_info.tag,
                     log_info.message);
}
}  // namespace

Logger::Logger() : log_stop_(false), log_thread_(&Logger::LogThreadRun, this) {}

Logger::~Logger() {
  {
    std::unique_lock lock(log_queue_mutex_);
    log_stop_ = true;
    log_queue_condition_variable_.notify_one();
  }
  log_thread_.join();
}

void Logger::Log(LogInfo log_info) {
#ifndef CRU_DEBUG
  if (log_info.level == LogLevel::Debug) {
    return;
  }
#endif

  std::unique_lock lock(log_queue_mutex_);
  log_queue_.push_back(std::move(log_info));
  log_queue_condition_variable_.notify_one();
}
void Logger::LogThreadRun() {
  while (true) {
    std::list<LogInfo> queue;
    bool stop = false;
    std::vector<ILogTarget *> target_list;

    {
      std::unique_lock lock(log_queue_mutex_);
      log_queue_condition_variable_.wait(
          lock, [this] { return !log_queue_.empty() || log_stop_; });
      std::swap(queue, log_queue_);
      stop = log_stop_;
    }

    {
      std::lock_guard<std::mutex> lock_guard(target_list_mutex_);
      for (const auto &target : target_list_) {
        target_list.push_back(target.get());
      }
    }

    for (const auto &target : target_list) {
      for (auto &log_info : queue) {
        target->Write(log_info.level, MakeLogFinalMessage(log_info));
      }
    }

    // TODO: Should still wait for queue to be cleared.
    if (stop) return;
  }
}

LoggerCppStream::LoggerCppStream(Logger *logger, LogLevel level,
                                 std::string tag)
    : logger_(logger), level_(level), tag_(std::move(tag)) {}

LoggerCppStream LoggerCppStream::WithLevel(LogLevel level) const {
  return LoggerCppStream(this->logger_, level, this->tag_);
}

LoggerCppStream LoggerCppStream::WithTag(std::string tag) const {
  return LoggerCppStream(this->logger_, this->level_, std::move(tag));
}

void LoggerCppStream::Consume(std::string_view str) {
  this->logger_->Log(this->level_, this->tag_, std::string(str));
}

}  // namespace cru::log
