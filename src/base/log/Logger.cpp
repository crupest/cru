#include "cru/base/log/Logger.h"
#include "cru/base/StringUtil.h"
#include "cru/base/log/StdioLogWriter.h"

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <ctime>
#include <format>
#include <memory>
#include <mutex>
#include <utility>

#ifdef _WIN32
#include "cru/base/platform/win/DebugLogWriter.h"
#endif

namespace cru::log {

namespace {
const char* LogLevelToString(LogLevel level) {
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
      std::unreachable();
  }
}

std::string GetLogTime() {
  auto time = std::time(nullptr);
  std::tm calendar{};
#ifdef _WIN32
  localtime_s(&calendar, &time);
#else
  localtime_r(&time, &calendar);
#endif
  return std::format("{}:{}:{}", calendar.tm_hour, calendar.tm_min,
                     calendar.tm_sec);
}
}  // namespace

std::string DefaultLogFormatter::Format(const LogInfo& log_info) {
  if (log_info.tag.empty()) {
    return std::format("[{}] {}: {}", GetLogTime(),
                       LogLevelToString(log_info.level), log_info.message);
  } else {
    return std::format("[{}] {} {}: {}", GetLogTime(),
                       LogLevelToString(log_info.level), log_info.tag,
                       log_info.message);
  }
}

ILogger* ILogger::GetInstance() {
  static SynchronousLogger logger;
  static bool initialized = [] {
    InstallDefaultWriters(&logger);
    logger.LoadDebugTagFromEnv();
    return true;
  }();
  CRU_UNUSED(initialized)
  return &logger;
}

void ILogger::InstallDefaultWriters(ILogger* logger) {
  logger->AddWriter(std::make_unique<StdioLogWriter>());

#ifdef _WIN32
  logger->AddWriter(std::make_unique<platform::win::WinDebugLogWriter>());
#endif
}

void ILogger::LoadDebugTagFromEnv(const char* env_var, std::string sep) {
  auto env = std::getenv(env_var);
  if (env != nullptr) {
    for (auto tag : cru::string::Split(std::string(env), sep)) {
      AddDebugTag(std::move(tag));
    }
  }
}

LoggerConfigurationMixin::LoggerConfigurationMixin()
    : config_(std::shared_ptr<LoggerConfig>(
          new LoggerConfig{std::make_shared<DefaultLogFormatter>(), {}, {}})) {}

void LoggerConfigurationMixin::AddDebugTag(std::string tag) {
  UpdateConfig([&tag](LoggerConfig* config) {
    config->debug_tags.insert(std::move(tag));
  });
}

void LoggerConfigurationMixin::RemoveDebugTag(const std::string& tag) {
  UpdateConfig([&tag](LoggerConfig* config) { config->debug_tags.erase(tag); });
}

void LoggerConfigurationMixin::SetFormatter(
    std::unique_ptr<ILogFormatter> formatter) {
  UpdateConfig([&formatter](LoggerConfig* config) {
    config->formatter = std::move(formatter);
  });
}

void LoggerConfigurationMixin::AddWriter(std::unique_ptr<ILogWriter> writer) {
  UpdateConfig([&writer](LoggerConfig* config) {
    config->writers.push_back(std::move(writer));
  });
}

void LoggerConfigurationMixin::RemoveWriter(ILogWriter* writer) {
  UpdateConfig([writer](LoggerConfig* config) {
    std::erase_if(config->writers,
                  [writer](const auto& w) { return w.get() == writer; });
  });
}

std::shared_ptr<LoggerConfigurationMixin::LoggerConfig>
LoggerConfigurationMixin::GetConfigSnapshot() {
  std::lock_guard lock(config_mutex_);
  return config_;
}

void SynchronousLogger::Log(LogInfo log_info) {
  auto config = GetConfigSnapshot();

  if (log_info.level == LogLevel::Debug &&
      std::ranges::none_of(config->debug_tags,
                           [&log_info](const std::string& tag) {
                             return log_info.tag.starts_with(tag);
                           })) {
    return;
  }
  auto message = config->formatter->Format(log_info);
  for (const auto& writer : config->writers) {
    writer->Write(log_info, message);
  }
}

AsynchronousLogger::AsynchronousLogger()
    : log_stop_(false), log_thread_(&AsynchronousLogger::LogThreadRun, this) {}

AsynchronousLogger::~AsynchronousLogger() {
  {
    std::unique_lock lock(mutex_);
    log_stop_ = true;
    condition_variable_.notify_all();
  }
  log_thread_.join();
}

void AsynchronousLogger::Log(LogInfo log_info) {
  std::unique_lock lock(mutex_);
  log_queue_.push_back(std::move(log_info));
  condition_variable_.notify_all();
}

void AsynchronousLogger::LogThreadRun() {
  std::list<LogInfo> queue;
  bool stop = false;

  while (true) {
    {
      std::unique_lock lock(mutex_);
      condition_variable_.wait(
          lock, [this] { return !log_queue_.empty() || log_stop_; });
      queue.swap(log_queue_);
      stop = log_stop_;
    }

    auto config = GetConfigSnapshot();
    for (const auto& writer : config->writers) {
      for (const auto& log_info : queue) {
        if (log_info.level == LogLevel::Debug &&
            std::ranges::none_of(config->debug_tags,
                                 [&log_info](const std::string& tag) {
                                   return log_info.tag.starts_with(tag);
                                 })) {
          continue;
        }
        writer->Write(log_info, config->formatter->Format(log_info));
      }
    }
    queue.clear();

    if (stop) return;
  }
}

Guard MeasureTimeAndLog(std::string_view tag, std::string_view name) {
  CruLogDebug(std::string(tag), "Start measure {}.", name);
  auto start = std::chrono::high_resolution_clock::now();

  return Guard([tag = std::string(tag), name = std::string(name), start] {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    CruLogDebug(tag, "End measure {}, time {} us.", name, duration.count());
  });
}
}  // namespace cru::log
