/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SANDBOX_LINUX_SECCOMP_BPF_HELPERS_SECCOMP_STARTER_OHOS_H_
#define SANDBOX_LINUX_SECCOMP_BPF_HELPERS_SECCOMP_STARTER_OHOS_H_

#include <memory>

#include "sandbox/linux/bpf_dsl/policy.h"
#include "sandbox/sandbox_buildflags.h"
#include "sandbox/sandbox_export.h"

namespace sandbox {

namespace bpf_dsl {
class Policy;
}

enum class SeccompSandboxStatus {
  NOT_SUPPORTED = 0,  // Seccomp is not supported.
  DETECTION_FAILED,   // Run-time detection of SeccompTSYNC failed.
  ENGAGED             // Sandbox was enabled and successfully turned on.
};

class SANDBOX_EXPORT SeccompStarterOhos {
 public:
  explicit SeccompStarterOhos();

  ~SeccompStarterOhos();

  void set_policy(std::unique_ptr<bpf_dsl::Policy> policy) {
    policy_ = std::move(policy);
  }

  bool StartSandbox();

  SeccompSandboxStatus status() const { return status_; }

 private:
  SeccompSandboxStatus status_ = SeccompSandboxStatus::NOT_SUPPORTED;
  std::unique_ptr<bpf_dsl::Policy> policy_;
};

}  // namespace sandbox
#endif  // SANDBOX_LINUX_SECCOMP_BPF_HELPERS_SECCOMP_STARTER_OHOS_H_
