#include "cru/common/logger.hpp"

#include "cru/common/format.hpp"

#include <cstdlib>
#include <ctime>
#include <memory>
#include <string_view>

namespace cru::log {
namespace {
Logger *CreateLogger() {
  const auto logger = new Logger();
  logger->AddSource(new StdioLoggerSource());
  return logger;
}
}  // namespace

Logger *Logger::GetInstance() {
  static std::unique_ptr<Logger> logger{CreateLogger()};
  return logger.get();
}

Logger::~Logger() {
  for (const auto i : sources_) {
    delete i;
  }
}

void Logger::AddSource(ILoggerSource *source) { sources_.push_back(source); }

void Logger::RemoveSource(ILoggerSource *source) { sources_.remove(source); }

namespace {
std::wstring_view LogLevelToString(LogLevel level) {
  switch (level) {
    case LogLevel::Debug:
      return L"DEBUG";
    case LogLevel::Info:
      return L"INFO";
    case LogLevel::Warn:
      return L"WARN";
    case LogLevel::Error:
      return L"ERROR";
    default:
      std::abort();
  }
}
}  // namespace

void Logger::Log(LogLevel level, const std::wstring_view &s) {
#ifndef CRU_DEBUG
  if (level == LogLevel::Debug) {
    return;
  }
#endif
  for (const auto source : sources_) {
    auto now = std::time(nullptr);
    wchar_t buffer[50];
    std::wcsftime(buffer, 50, L"%c", std::localtime(&now));

    source->Write(level, util::Format(L"[{}] {}: {}\n", buffer,
                                      LogLevelToString(level), s));
  }
}
}  // namespace cru::log
