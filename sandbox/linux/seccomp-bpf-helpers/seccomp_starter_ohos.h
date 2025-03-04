// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
