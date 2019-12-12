#include "cru/common/logger.hpp"

#include "cru/common/format.hpp"

#include <array>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <string_view>

namespace cru::log {
namespace {
Logger *CreateLogger() {
  const auto logger = new Logger();
  logger->AddSource(std::make_unique<StdioLogSource>());
  return logger;
}
}  // namespace

Logger *Logger::GetInstance() {
  static std::unique_ptr<Logger> logger{CreateLogger()};
  return logger.get();
}

void Logger::AddSource(std::unique_ptr<ILogSource> source) {
  sources_.push_back(std::move(source));
}

void Logger::RemoveSource(ILogSource *source) {
  sources_.remove_if([source](const std::unique_ptr<ILogSource> &s) {
    return s.get() == source;
  });
}

namespace {
std::string_view LogLevelToString(LogLevel level) {
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
      std::abort();
  }
}
}  // namespace

void Logger::Log(LogLevel level, const std::string_view &s) {
#ifndef CRU_DEBUG
  if (level == LogLevel::Debug) {
    return;
  }
#endif
  for (const auto &source : sources_) {
    auto now = std::time(nullptr);
    std::array<char, 50> buffer;
    std::strftime(buffer.data(), 50, "%c", std::localtime(&now));

    source->Write(level, util::Format("[{}] {}: {}\n", buffer.data(),
                                      LogLevelToString(level), s));
  }
}
}  // namespace cru::log
