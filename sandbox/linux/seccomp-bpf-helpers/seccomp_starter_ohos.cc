#include "sandbox/linux/seccomp-bpf-helpers/seccomp_starter_ohos.h"
#include <signal.h>
#include "base/logging.h"
#include "sandbox/linux/seccomp-bpf/sandbox_bpf.h"

namespace sandbox {

SeccompStarterOhos::SeccompStarterOhos() {}

SeccompStarterOhos::~SeccompStarterOhos() = default;

bool SeccompStarterOhos::StartSandbox() {
  // Do run-time detection to ensure that support is present.
  if (!SandboxBPF::SupportsSeccompSandbox(
          SandboxBPF::SeccompLevel::MULTI_THREADED)) {
    status_ = SeccompSandboxStatus::DETECTION_FAILED;
    LOG(WARNING) << "Seccomp support should be present, but detection "
                 << "failed. Continuing without Seccomp-BPF.";
    return false;
  }

  SandboxBPF sandbox(std::move(policy_));
  CHECK(sandbox.StartSandbox(SandboxBPF::SeccompLevel::MULTI_THREADED));
  status_ = SeccompSandboxStatus::ENGAGED;
  return true;
}

}  // namespace sandbox