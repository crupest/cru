#include "cru/common/SubProcess.h"

#ifdef CRU_PLATFORM_UNIX
#include "cru/common/platform/unix/PosixSpawnSubProcess.h"
#endif

namespace cru {

#ifdef CRU_PLATFORM_UNIX
using ThisPlatformSubProcess = platform::unix::PosixSpawnSubProcess;
#endif

SubProcess SubProcess::Create(String program, std::vector<String> arguments,
                              std::unordered_map<String, String> environments) {
  SubProcessStartInfo start_info;
  start_info.program = std::move(program);
  start_info.arguments = std::move(arguments);
  start_info.environments = std::move(environments);
  return SubProcess(std::move(start_info));
}

SubProcess::SubProcess(SubProcessStartInfo start_info) {
  platform_process_.reset(new ThisPlatformSubProcess(std::move(start_info)));
  platform_process_->Start();
}

SubProcess::~SubProcess() {}

void SubProcess::Wait(std::optional<std::chrono::milliseconds> wait_time) {
  CheckValid();
  platform_process_->Wait(wait_time);
}

void SubProcess::Kill() {
  CheckValid();
  platform_process_->Kill();
}

SubProcessStatus SubProcess::GetStatus() {
  CheckValid();
  return platform_process_->GetStatus();
}

SubProcessExitResult SubProcess::GetExitResult() {
  CheckValid();
  return platform_process_->GetExitResult();
}

io::Stream* SubProcess::GetStdinStream() {
  CheckValid();
  return platform_process_->GetStdinStream();
}

io::Stream* SubProcess::GetStdoutStream() {
  CheckValid();
  return platform_process_->GetStdoutStream();
}

io::Stream* SubProcess::GetStderrStream() {
  CheckValid();
  return platform_process_->GetStderrStream();
}

void SubProcess::Detach() { auto p = platform_process_.release(); }

void SubProcess::CheckValid() const {
  if (!IsValid()) {
    throw SubProcessException(u"SubProcess instance is invalid.");
  }
}

}  // namespace cru
