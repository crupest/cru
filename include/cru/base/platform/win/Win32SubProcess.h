#pragma once

#ifndef _WIN32
#error "This file can only be included on Windows."
#endif

#include "../../SubProcess.h"
#include "../../io/AutoReadStream.h"
#include "Base.h"
#include "Stream.h"

namespace cru::platform::win {
class CRU_BASE_API Win32SubProcessImpl
    : public Object,
      public virtual IPlatformSubProcessImpl {
 private:
  constexpr static auto kLogTag = "cru::platform::win::Win32SubProcessImpl";

 public:
  explicit Win32SubProcessImpl();
  ~Win32SubProcessImpl();

  void PlatformCreateProcess(const SubProcessStartInfo& start_info) override;
  SubProcessExitResult PlatformWaitForProcess() override;
  void PlatformKillProcess() override;

  io::Stream* GetStdinStream() override;
  io::Stream* GetStdoutStream() override;
  io::Stream* GetStderrStream() override;

 private:
  PROCESS_INFORMATION process_;
  int exit_code_;

  std::unique_ptr<Win32HandleStream> stdin_stream_;
  std::unique_ptr<Win32HandleStream> stdout_stream_;
  std::unique_ptr<Win32HandleStream> stderr_stream_;

  std::unique_ptr<io::AutoReadStream> stdout_buffer_stream_;
  std::unique_ptr<io::AutoReadStream> stderr_buffer_stream_;
};
}  // namespace cru::platform::win
